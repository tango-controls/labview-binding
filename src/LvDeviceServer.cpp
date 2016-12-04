// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   LvDeviceServer.cpp
//
// = AUTHOR
//   Nicolas Leclercq - Synchrotron SOLEIL - France
//
// ============================================================================

#include "Config.h"

#if defined (_TBFL_HAS_DEVICE_SERVER_SUPPORT_)

//=============================================================================
// DEPENDENCIES
//=============================================================================
#include <iomanip>
#include <algorithm>
#include "CommonHeader.h"
#include <yat/file/FileName.h>
#include "LvDeviceProxy.h"
#include "DataAdapter.h"
#include "LvDeviceImpl.h"
#include "LvDevice.h"
#include "LvDeviceClass.h"
#include "LvDeviceClassFactory.h"
#include "LvDeviceRepository.h"
#include "LvDeviceServer.h"

#if !defined (_LV_INLINE_)
  #include "LvDeviceServer.i"
#endif

//=============================================================================
// STRUCT : LvTangoKernelThread
//=============================================================================
class LvTangoKernelThread : public yat::Thread
{
  friend class LvDeviceServer;

public:
  static LvTangoKernelThread *instance ();
  //- returns the unique instance of <LvTangoKernelThread>.

  void exit ();
  //- mandatory exit impl.

protected:
  yat::Thread::IOArg run_undetached (yat::Thread::IOArg);
  //- the thread entry point - calle by yat::Thread::start_undetached

private:
  static int init ();
  //- Instanciates the singleton.

  static void cleanup ();
  //- Releases the singleton.

  static bool tango_event_loop ();
  //- Define our own event loop

  static LvTangoKernelThread* instance_;
  //- The unique instance of <LvTangoKernelThread>

  bool go_on_;
  //- thread execution control flag

  void wait_thread_properly_spawned ();
  bool spawned_;
  yat::Mutex cond_lock_;
  yat::Condition cond_var_;

  // = Disallow these operations (except for ObjectManager).
  //-------------------------------------------------------
  LvTangoKernelThread();
  LvTangoKernelThread (const LvTangoKernelThread&);
  virtual ~LvTangoKernelThread();
  void operator= (const LvTangoKernelThread&);
};

//=============================================================================
// STATIC MEMBERS
//=============================================================================
LvTangoKernelThread* LvTangoKernelThread::instance_ = 0;
//-----------------------------------------------------------------------------
LvDeviceServer* LvDeviceServer::instance_ = 0;
yat::Mutex LvDeviceServer::lock_;
//-----------------------------------------------------------------------------

//=============================================================================
// LvTangoKernelThread::init
//=============================================================================
int LvTangoKernelThread::init ()
{
  if ( LvTangoKernelThread::instance_ )
    return 0;

  LvTangoKernelThread::instance_ = new LvTangoKernelThread;

  if ( LvTangoKernelThread::instance_ )
  {
    LvTangoKernelThread::instance_->start_undetached();
    yat::Thread::yield();
    LvTangoKernelThread::instance_->wait_thread_properly_spawned();
  }

  return LvTangoKernelThread::instance_ ? 0 : -1;
}

//=============================================================================
// LvTangoKernelThread::cleanup
//=============================================================================
void LvTangoKernelThread::cleanup ()
{
  if ( LvTangoKernelThread::instance_ )
  {
    LvTangoKernelThread::instance_->exit();
    //- we should normally call 'join' on the underlying thread but a call
    //- generates a crash at exit (Tango internals?)- not a big deals since
    //- the application is quitting...
    LvTangoKernelThread::instance_ = 0;
  }
}

//=============================================================================
// LvTangoKernelThread::instance
//=============================================================================
LV_INLINE LvTangoKernelThread* LvTangoKernelThread::instance ()
{
  return LvTangoKernelThread::instance_;
}

//=============================================================================
// LvTangoKernelThread::LvTangoKernelThread
//=============================================================================
LvTangoKernelThread::LvTangoKernelThread ()
  : yat::Thread(),
    go_on_(true),
    spawned_(false),
    cond_lock_(),
    cond_var_(cond_lock_)
{
  //- noop
}

//=============================================================================
// LvTangoKernelThread::LvTangoKernelThread
//=============================================================================
LvTangoKernelThread::~LvTangoKernelThread ()
{
  //- noop
}

//=============================================================================
// LvTangoKernelThread::wait_thread_properly_spawned
//=============================================================================
void LvTangoKernelThread::wait_thread_properly_spawned ()
{
  yat::MutexLock guard(cond_lock_);
  if ( ! spawned_ )
  {
    //- here we could be woken up by a kernel event (signal), anyway...
    bool notified_in_time = cond_var_.timed_wait(10000);
    if ( ! notified_in_time )
      TBFL_STDOUT("LvTangoKernelThread::wait_thread_properly_spawned::tmo expired while waiting for the ORB thread to start");
    else
      TBFL_STDOUT("LvTangoKernelThread::wait_thread_properly_spawned::ok, ORB thread properly spawned");
  }
}

//=============================================================================
// LvTangoKernelThread::exit
//=============================================================================
void LvTangoKernelThread::exit ()
{
  TBFL_TRACE("LvTangoKernelThread::exit");
  
  go_on_ = false;
}

//=============================================================================
// LvTangoKernelThread::tango_event_loop
//=============================================================================
bool LvTangoKernelThread::tango_event_loop ()
{
  bool go_on = LvTangoKernelThread::instance()->go_on_;

  if ( go_on )
  {
    if ( ! LvTangoKernelThread::instance()->spawned_ )
    {
      yat::MutexLock guard(LvTangoKernelThread::instance()->cond_lock_);
      LvTangoKernelThread::instance()->spawned_ = true;
      LvTangoKernelThread::instance()->cond_var_.signal();
    }

    //- avoid burning the CPU!
    yat::Thread::sleep(250);
  }

  return ! go_on;
}

//=============================================================================
// LvTangoKernelThread::run_undetached
//=============================================================================
yat::Thread::IOArg LvTangoKernelThread::run_undetached (yat::Thread::IOArg)
{
  TBFL_TRACE(this);

  try
  {
    if ( ! LvDeviceServer::instance() )
    {
      TBFL_STDOUT("LvTangoKernelThread::run_undetached::the LvDeviceServer is not properly initialized!");
      return 0;
    }

    //- get tango device server name
    std::string executable_name;
    std::string instance_name;
    LvDeviceServer::instance()->device_server_name(executable_name, instance_name);

    //- pseudo command line args: argv
    std::string argv_1(executable_name);
    std::string argv_2(instance_name);
    std::string argv_3("-v1");
    char *argv[] = {
        const_cast<char*>(argv_1.c_str()),
        const_cast<char*>(argv_2.c_str()),
        const_cast<char*>(argv_3.c_str()),
        NULL
    };
    
    //- pseudo command line args: argc
    int argc = sizeof(argv) / sizeof(char*) - 1;

    //- tango utils initialization
    TBFL_STDOUT("LvTangoKernelThread::run_undetached::initializing Tango::Util");
    Tango::Util *tg = Tango::Util::init(argc, argv);

    //- register our class factory
    TBFL_STDOUT("LvTangoKernelThread::run_undetached::registering the class factory");
    Tango::DServer::register_class_factory(lv_binding_class_factory);
  
#if ! defined(_TBFL_WINDOWS_)
    //- user defined event loop (advanced Tango feature, not sure that any other project use it on earth!)
    //- doesn't work on windows :-{
    TBFL_STDOUT("LvTangoKernelThread::run_undetached::registering event loop (linux impl.)");
    tg->server_set_event_loop(LvTangoKernelThread::tango_event_loop);
#endif

    //- tango server initialization
    TBFL_STDOUT("LvTangoKernelThread::run_undetached::initializing the device server");
    tg->server_init(false);

#if defined(_TBFL_WINDOWS_)
    //- user defined event loop doesn't work on windows :-{   
    {
      TBFL_STDOUT("LvTangoKernelThread::run_undetached::registering event loop (windows impl.)");
      yat::MutexLock guard(LvTangoKernelThread::instance()->cond_lock_);
      LvTangoKernelThread::instance()->spawned_ = true;
      LvTangoKernelThread::instance()->cond_var_.signal();
    }
#endif

    //- enter CORBA events loop (will not return until the server is asked to exit)
    TBFL_STDOUT("LvTangoKernelThread::run_undetached::calling Tango::Util::server_run (entering server events loop)");
    tg->server_run();
    TBFL_STDOUT("LvTangoKernelThread::run_undetached::Tango::Util::server_run returned (server is about to exit)");

    TBFL_STDOUT("LvTangoKernelThread::run_undetached::calling Tango::Util::server_cleanup");
    Tango::Util::instance()->server_cleanup();

    TBFL_STDOUT("LvTangoKernelThread::run_undetached::bye!");
  }
  catch (bad_alloc &)
  {
    TBFL_STDOUT("LvTangoKernelThread::std::bad_alloc exception caught! Thread exiting...");
  }
  catch (CORBA::Exception &e)
  {
    Tango::Except::print_exception(e);
    TBFL_STDOUT("LvTangoKernelThread::CORBA exception caught! Thread exiting...");
  }
  catch (...)
  {
    TBFL_STDOUT("unknown exception caught! Thread exiting...");
  }

  return 0;
}

//=============================================================================
// STRUCT : AsynchronousTasks
//=============================================================================
class AsynchronousTasks : public yat::Thread 
{
  friend class LvDeviceServer;

  typedef enum
  {
    ASYNC_JOB_EMPTY_TRASHES = 0,
    ASYNC_JOB_DO_NOTHING
  } AsyncJobIdentifier;

public:
  AsynchronousTasks()
    : go_on_(true),
      cond_lock_(),
      cond_var_(cond_lock_),
      async_job(ASYNC_JOB_DO_NOTHING)
  {
    //- noop
  }

  void exit ()
  {
    yat::MutexLock guard(cond_lock_);
    go_on_ = false;
    cond_var_.signal();
  };

protected:
  yat::Thread::IOArg run_undetached (yat::Thread::IOArg)
  {
    yat::MutexLock guard(cond_lock_);
    yat::Timeout tmo(10, yat::Timeout::TMO_UNIT_SEC, true);
    while ( go_on_ )
    {
      bool signaled = cond_var_.timed_wait(1000);
      if ( go_on_ &&  LvDeviceServer::instance() )
      {
        switch ( async_job )
        {
          case ASYNC_JOB_DO_NOTHING:
          default:
            if ( tmo.expired() )
            {
             _BEST_EFFORT_TRY_(LvDeviceServer::instance()->empty_client_request_trashes());
             tmo.restart();
            }
            break;
        }
      }
    }
    return 0;
  }

private:
  bool go_on_;
  yat::Mutex cond_lock_;
  yat::Condition cond_var_;
  AsyncJobIdentifier async_job;
};


//=============================================================================
// LvDeviceServer::init
//=============================================================================
void LvDeviceServer::init (const std::string& executable_name,
                           const std::string& instance_name,
                           LVUserEventRef ds_evt,
                           LvStringArrayHdl device_list)
    throw (Tango::DevFailed)
{
  TBFL_TRACE("welcome");
  
  //- critical section
  yat::MutexLock guard(LvDeviceServer::lock_);

  try
  {
    if ( ! LvDeviceServer::instance_ )
    {
      LvDeviceServer::instance_ = new LvDeviceServer;
      if ( ! LvDeviceServer::instance_ )
        throw std::bad_alloc();

      LvDeviceRepository::init();
    }

    LvDeviceServer::instance_->prepare_to_run(executable_name, instance_name, ds_evt, device_list);
     
    LvDeviceServer::instance_->start_devices();
  }
  catch (const std::bad_alloc&)
  {
    Tango::Except::throw_exception(_CPTC_("memory error"),
                                   _CPTC_("LvDeviceServer initialization failed"),
                                   _CPTC_("LvDeviceServer::init"));
  }
  catch (Tango::DevFailed& df)
  {
    Tango::Except::re_throw_exception(df,
                                      _CPTC_("software error"),
                                      _CPTC_("LvDeviceServer initialization failed [Tango exception caught]"),
                                      _CPTC_("LvDeviceServer::init"));
  }
  catch (...)
  {
    Tango::Except::throw_exception(_CPTC_("software error"),
                                   _CPTC_("LvDeviceServer initialization failed [unknown exception caught]"),
                                   _CPTC_("LvDeviceServer::init"));
  }
}

//=============================================================================
// LvDeviceServer::cleanup
//=============================================================================
void LvDeviceServer::cleanup ()
    throw (Tango::DevFailed)
{
  TBFL_TRACE("bye");
  
  yat::MutexLock guard(LvDeviceServer::lock_);

  if ( LvDeviceServer::instance_ )
  {
    LvDeviceServer::instance_->abort_client_requests();

    _BEST_EFFORT_TRY_(LvDeviceServer::instance_->stop_devices(true));

    _BEST_EFFORT_TRY_(LvTangoKernelThread::cleanup());

    _BEST_EFFORT_TRY_(LvDeviceRepository::cleanup());
       
    delete LvDeviceServer::instance_;
    LvDeviceServer::instance_ = 0;
  }
}

//=============================================================================
// LvDeviceServer::empty_client_request_trashes
//=============================================================================
void LvDeviceServer::empty_client_request_trashes ()
{
  LvDeviceByNameMapIterator it = LvDeviceServer::instance_->devices_by_name_.begin();
  for ( ; it != LvDeviceServer::instance_->devices_by_name_.end(); ++it )
  {
    if ( it->second )
    {
      _BEST_EFFORT_TRY_(it->second->empty_client_request_trash());
    }
  }
}

//=============================================================================
// LvDeviceServer::LvDeviceServer
//=============================================================================
LvDeviceServer::LvDeviceServer ()
  : device_server_(0),
    async_tasks_(0),
    tango_is_running_(false)
{
  TBFL_TRACE(this);

  async_tasks_ = new AsynchronousTasks;
  async_tasks_->start_undetached();
}

//=============================================================================
// LvDeviceServer::~LvDeviceServer
//=============================================================================
LvDeviceServer::~LvDeviceServer ()
{
  TBFL_TRACE(this);

  device_server_ = 0;
  
  _BEST_EFFORT_TRY_(cleanup_devices_repository());

  if ( async_tasks_ )
  {
    async_tasks_->exit();
    async_tasks_->join(0);
  }
}

//=============================================================================
// LvDeviceServer::get_tango_db_address
//=============================================================================
void LvDeviceServer::get_tango_db_address (std::string& host, int& port)
  throw (Tango::DevFailed)
{
  std::string tango_host;
  int err = Tango::ApiUtil::get_env_var("TANGO_HOST", tango_host);
  if ( err )
  {
    Tango::Except::throw_exception(_CPTC_("internal error"),
                                   _CPTC_("no Tango database available [is the TANGO_HOST env. var. properly defined?]"),
                                   _CPTC_("LvDeviceServer::get_tango_db_address"));
  }

  tbfl::size_t p1 = tango_host.find(":");
  if ( p1 == std::string::npos )
  {
    Tango::Except::throw_exception(_CPTC_("internal error"),
                                   _CPTC_("TANGO_HOST env. var. is not properly set [syntax error]"),
                                   _CPTC_("LvDeviceServer::get_tango_db_address"));
  }

  tbfl::size_t p2 = tango_host.find(",");
  p2 = ( p2 == std::string::npos ) ? tango_host.size() : --p2;

  host = tango_host.substr(0, p1);
  port = ::atoi(tango_host.substr(p1 + 1, p2 - p1 ).c_str());
}

//=============================================================================
// LvDeviceServer::get_device_server_info
//=============================================================================
void LvDeviceServer::get_device_server_info (const std::string& executable,
                                             const std::string& instance,
                                             LvDeviceServerCache& dsi)
  throw (Tango::DevFailed)
{
  TBFL_TRACE(this);
  
  try
  {
    //- clear device server cache
    dsi.clear();

    //- store executable & instance name
    dsi.executable_name = executable;
    dsi.instance_name = instance;
  
    //- build connection to the Tango database
    int port;
    std::string host;
    get_tango_db_address(host, port);
    Tango::Database db(host, port);

    //- get devices attached to the specified device server instance
    std::string ds(executable + "/" + instance);
    get_devices_list(ds, dsi, &db);

    //- let's use some aliases...
    LvServerClassesMap& cl = dsi.ds_class_to_devices;
    LvServerDevicesMap& dl = dsi.ds_device_to_class;
    
    //- get device class properties
    LvServerClassesMapConstIterator cit = cl.begin();
    for ( ; cit != cl.end(); ++cit )
    {
      //- get device class property list
      std::vector<std::string> cpl;
      try
      {
        Tango::DbDatum pl = db.get_class_property_list(const_cast<string&>(cit->first));
        pl >> cpl;
      }
      catch (Tango::DevFailed& df)
      {
        std::ostringstream oss;
        oss << "failed to obtain device class property list from Tango database for class "
            << cit->first;
        Tango::Except::re_throw_exception(df,
                                          _CPTC_("configuration error"),
                                          _CPTC_(oss.str().c_str()),
                                          _CPTC_("LvDeviceServer::get_device_server_info"));
      }
#if defined(_TBFL_VERBOSE_)
      {
        ostringstream oss;
        oss << "device class " << cit->first << " has the following properties defined: ";
        for ( tbfl::size_t i = 0; i < cpl.size(); i++ )
        {
          oss << cpl[i] << ( i < cpl.size() - 1 ? ", " : "" );
        }
        TBFL_STDOUT(oss.str());
      }
#endif
      //- build list of properties to read from Tango database
      Tango::DbData dbd;
      for ( tbfl::size_t i = 0; i < cpl.size(); i++ )
      {
        dbd.push_back(Tango::DbDatum(cpl[i]));
      }
      //- get data from Tango database
      try
      {
        db.get_class_property(const_cast<std::string&>(cit->first), dbd);
      }
      catch (Tango::DevFailed& df)
      {
        std::ostringstream oss;
        oss << "failed to obtain device class properties for class "
            << cit->first;
        Tango::Except::re_throw_exception(df,
                                          _CPTC_("configuration error"),
                                          _CPTC_(oss.str().c_str()),
                                          _CPTC_("LvDeviceServer::get_device_server_info"));
      }
      //- store properties values into the device server cache
      for ( tbfl::size_t i = 0; i < cpl.size(); i++ )
      {
        std::vector<std::string> prop_value;
        if ( ! ( dbd[i] >> prop_value ) )
        {
          std::ostringstream oss;
          oss << "failed to extract value of device class property '"
              << cpl[i]
              << "' for class "
              << cit->first
              << " [please check the Tango database]";
          Tango::Except::throw_exception(_CPTC_("configuration error"),
                                        _CPTC_(oss.str().c_str()),
                                        _CPTC_("LvDeviceServer::get_device_server_info"));
        }
        //- store devcie class property value into our cache
        dsi.ds_class_properties[cit->first][cpl[i]] = prop_value;
#if defined(_TBFL_VERBOSE_)
        for ( tbfl::size_t j = 0; j < prop_value.size(); j++ )
        {
          ostringstream oss;
          oss << "device class property " << cit->first << "[" << cpl[i] << "]";
          oss << "[" << j + 1 << "/" << prop_value.size() << "] = " << prop_value[j];
          TBFL_STDOUT(oss.str());
        }
#endif
      }
    }

    //- each LabVIEW device class must have (at least) the 'PathToPogoXmiFile' property defined
    //- no need to go further in case the this property is missing
    std::string xmi_path_prop("PathToPogoXmiFile");

    //- any bad class? (property not defined)
    std::vector<std::string> bad_classes;
    cit = cl.begin();
    for ( ; cit != cl.end(); ++cit )
    {
      LvTangoPropertiesMapConstIterator it = dsi.ds_class_properties[cit->first].find(xmi_path_prop);
      if ( it == dsi.ds_class_properties[cit->first].end() ||  it->second.empty() || it->second.size() != 1 )
      {
        bad_classes.push_back(cit->first);
      }
      else
      {
        yat::FileName xmi_file(it->second[0]);
        if ( ! xmi_file.file_exist() )
          bad_classes.push_back(cit->first);
      }
    }
    if ( ! bad_classes.empty() )
    {
      std::ostringstream oss;
      oss << "the "
          << xmi_path_prop
          << " property is either missing or invalid (e.g. bad path) for the following device class(es): ";
      for ( tbfl::size_t i = 0; i < bad_classes.size(); i++ )
      {
        oss << bad_classes[i] << ( i < bad_classes.size() - 1 ? ", " : "" );
      }
      oss << " [each LabVIEW device class must have this class property defined]";
      Tango::Except::throw_exception(_CPTC_("configuration error"),
                                     _CPTC_(oss.str().c_str()),
                                     _CPTC_("LvDeviceServer::get_device_server_info"));
    }

    //- parse the device class interface
    parse_device_class_interfaces(dsi);
  }
  catch ( ... )
  {
    //- clear device server cache
    dsi.clear();
    throw;
  }
}

//=============================================================================
// LvDeviceServer::cleanup_devices_repository
//=============================================================================
void LvDeviceServer::cleanup_devices_repository ()
{
  LvDeviceByNameMapIterator it = devices_by_name_.begin();
  for (; it != devices_by_name_.end(); ++it )
  {
    delete it->second;
  }
  devices_by_name_.clear();
}

//=============================================================================
// LvDeviceServer::device_server_name
//=============================================================================
std::string LvDeviceServer::device_server_name () const
{
  return device_server_cache_.executable_name + "/" + device_server_cache_.instance_name;
}

//=============================================================================
// LvDeviceServer::device_server_name
//=============================================================================
void LvDeviceServer::device_server_name (std::string& exec_name, std::string& inst_name) const
{
  exec_name = device_server_cache_.executable_name;
  inst_name = device_server_cache_.instance_name;
}

//=============================================================================
// LvDeviceServer::build_lv_device_list
//=============================================================================
void LvDeviceServer::build_lv_device_list (LvStringArrayHdl device_list)
    throw (Tango::DevFailed)
{
  tbfl::size_t n = 2 * device_server_cache_.ds_device_to_class.size();

  MgErr err = ::NumericArrayResize(LvMemoryAddress,
                                   1L,
                                   reinterpret_cast<UHandle*>(&device_list),
                                   n);
  if (err != noErr)
  {
    Tango::Except::throw_exception(_CPTC_("memory error"),
                                   _CPTC_("NumericArrayResize failed"),
                                   _CPTC_("LvDeviceServer::build_lv_device_list"));
  }

  (*device_list)->length = n;
  ::memset((*device_list)->data, 0, (*device_list)->length * sizeof(void*));
  tbfl::size_t l = 0;
  LvStringHdl sh;
  LvServerDevicesMapConstIterator dit = device_server_cache_.ds_device_to_original_class.begin();
  for (tbfl::size_t i = 0, j = 0; i < (*device_list)->length; i += 2)
  {
    //- device name -----------------------------------------
    sh = 0;
    DATA_ADAPTER->to_lv_str(dit->first.c_str(), sh);
    (*device_list)->data[i] = sh;
    //- class name -----------------------------------------
    sh = 0;
    DATA_ADAPTER->to_lv_str(dit->second.c_str(), sh);
    (*device_list)->data[i + 1] = sh;
    //- next -----------------------------------------------
    ++dit;
  }
}

//=============================================================================
// LvDeviceServer::prepare_to_run
//=============================================================================
void LvDeviceServer::prepare_to_run (const std::string& executable_name,
                                     const std::string& instance_name,
                                     LVUserEventRef ds_evt,
                                     LvStringArrayHdl device_list)
    throw (Tango::DevFailed)
{
  TBFL_TRACE(this);
  
  try
  {
    //- get device server info (populate the device server cache)
    LvDeviceServerCache dsi;
    get_device_server_info(executable_name, instance_name, dsi);

    //- important note: having two (or more) devices of the same device class adding
    //- the same dynamic interface causes a lot of troubles - since the interface
    //- of our LabVIEW devices is purely dynamic, we need to implement a workaround
    //- in order to be able to run two (or more) devices of the same class into
    //- the LabVIEW device server. the idea is to instance a "virtual" class per
    //- device - the trick is purely local and is supposed to have no side effect
    //- on Tango kernel and/or any client of the LabVIEW devices
    apply_per_device_virtual_class(dsi);
  
    //- can't add or remove devices if the device server has already been started successfully
    //- this is a Tango kernel constraint due to the fact that Tango as NOT been designed to
    //- run in the context of a DLL (or shared lib) - we are simply crazy enough to do it...
    if  ( tango_is_running_ )
    {
      bool not_same_exec = device_server_cache_.executable_name != executable_name;
      bool not_same_inst = device_server_cache_.instance_name != instance_name;
      if ( not_same_exec || not_same_inst )
      {
        ostringstream oss;
        oss << "a LabVIEW device server is already running - for technical reasons, we can only run one Tango "
            << "device server per LabVIEW instance or application";
        Tango::Except::throw_exception(_CPTC_("runtime conflict"),
                                       _CPTC_(oss.str().c_str()),
                                       _CPTC_("LvDeviceServer::prepare_to_run"));
      }
      LvServerDevicesMapIterator it = dsi.ds_device_to_class.begin();
      for ( ; it != dsi.ds_device_to_class.end(); ++it )
      {
        bool dev_not_found = device_server_cache_.ds_device_to_class.find(it->first) == device_server_cache_.ds_device_to_class.end();
        if ( dev_not_found )
        {
          ostringstream oss;
          oss << "for technical reasons, adding a Tango device to a running LabVIEW device server requires "
              << "the application to be restarted - please restart LabVIEW then retry,";
          Tango::Except::throw_exception(_CPTC_("runtime conflict"),
                                         _CPTC_(oss.str().c_str()),
                                         _CPTC_("LvDeviceServer::prepare_to_run"));
        }
        TBFL_STDOUT("LvDeviceServer::prepare_to_run::does Tango class "
                    << it->second
                    << " belong to the existing device server?");
        bool dev_class_not_found = device_server_cache_.ds_class_to_devices.find(it->second) == device_server_cache_.ds_class_to_devices.end();
        if (  dev_class_not_found )
        {
          ostringstream oss;
          oss << "for technical reasons, changing the class of a Tango device attached to a running LabVIEW device server requires "
              << "the application to be restarted - please restart LabVIEW then retry,";
          Tango::Except::throw_exception(_CPTC_("runtime conflict"),
                                         _CPTC_(oss.str().c_str()),
                                         _CPTC_("LvDeviceServer::prepare_to_run"));
        }
      }
    }

    //- copy device server info
    device_server_cache_.clear();
    device_server_cache_ = dsi;

    //- anything registered in this device server?
    if ( device_server_cache_.ds_device_to_class.empty() )
    {
      ostringstream oss;
      oss << "there is no Tango device registered for device server " << executable_name << "/" << instance_name;
      Tango::Except::throw_exception(_CPTC_("invalid configuration"),
                                     _CPTC_(oss.str().c_str()),
                                     _CPTC_("LvDeviceServer::prepare_to_run"));
    }
    
    //- store the device serve event ref.
    device_server_cache_.ds_event = ds_evt;
    
    //- return device list to labview
    build_lv_device_list(device_list);
  }
  catch (...)
  {
    if ( ! tango_is_running_ )
       cleanup_devices_repository();
    classes_by_names_.clear();
    throw;
  }
}

//=============================================================================
// LvDeviceServer::apply_per_device_virtual_class
//=============================================================================
void LvDeviceServer::apply_per_device_virtual_class (LvDeviceServerCache& dsi)
    throw (Tango::DevFailed)
{
  TBFL_TRACE(this);

  //- nothing to do if only one device in the device server
  if ( dsi.ds_device_to_class.size() <= 1 )
  {
    return;
  }

  //- nothing to do if there is only one device per class
  std::vector<std::string> cl;
  LvServerClassesMapConstIterator it = dsi.ds_class_to_devices.begin();
  for ( ; it != dsi.ds_class_to_devices.end(); ++it )
  {
    //- more than one device for class named 'it->first'?
    if ( it->second.size() > 1 )
    {
      cl.push_back(it->first);
    }
  }
  
  //- more than one device for class named 'it->first'?
  if ( cl.empty() )
  {
    return;
  }

  //- for each class with more than one device instance...
  for ( tbfl::size_t c = 0; c < cl.size(); c++ )
  {
    //- list of devices belonging to the same class
    std::vector<std::string>& dl = dsi.ds_class_to_devices[cl[c]];
    //- for each device of this list...
    for ( tbfl::size_t d = 0; d < dl.size(); d++ )
    {
      //- generate a virtual class name
      std::string ocn(cl[c]);
      std::transform(ocn.begin(), ocn.end(),ocn.begin(), ::toupper);
      std::ostringstream oss;
      oss << "LABVIEW_VCLASS_" << std::setfill('0') << std::setw(3) << d;
      TBFL_STDOUT("device "
                  << dl[d]
                  << " is now associated to virtual class "
                  << oss.str());
      //- change device class
      dsi.ds_device_to_class[dl[d]] = oss.str();
      //- create an entry for the virtual class in 'class to devices'
      dsi.ds_class_to_devices[oss.str()].push_back(dl[d]);
      //- create an entry for the virtual class in 'class properties' (copy data from original class)
      dsi.ds_class_properties[oss.str()] = dsi.ds_class_properties[cl[c]];
      //- create an entry for the virtual class in 'class interface' (copy data from original class)
      dsi.ds_class_interface[oss.str()] = dsi.ds_class_interface[cl[c]];
    }
    //- no more devices belonging to class cl[c]!
    dsi.ds_class_to_devices[cl[c]].clear();
  }
}

//=============================================================================
// LvDeviceServer::get_devices_list
//=============================================================================
void LvDeviceServer::get_devices_list (const std::string& ds, LvDeviceServerCache& dsi, Tango::Database* tdb)
  throw (Tango::DevFailed)
{
  Tango::Database * db = 0;

  LvServerClassesMap& cl = dsi.ds_class_to_devices;
  LvServerDevicesMap& dl = dsi.ds_device_to_class;
  LvServerDevicesMap& oc = dsi.ds_device_to_original_class;
    
  try
  {
    if ( ! tdb )
    {
      std::string db_host;
      int db_port;

      get_tango_db_address(db_host, db_port);

      TBFL_STDOUT("Contacting Tango db @ "
                  << db_host
                  << " on port "
                  << db_port);
                  
      db = new Tango::Database(db_host, db_port);
    }
    else
    {
      db = tdb;
    }

    Tango::DbDatum dd = db->get_device_class_list(const_cast<std::string&>(ds));
    std::vector<std::string> dsl;
    dd >> dsl;

    if ( dsl.size() % 2 )
    {
      ostringstream oss;
      oss << "Tango::Database::get_device_class_list returned an unexpected response format!"
          << " [something changed in Tango implementation?]";
      Tango::Except::throw_exception(_CPTC_("configuration error"),
                                     _CPTC_(oss.str().c_str()),
                                     _CPTC_("LvDeviceServer::get_devices_list"));
    }

    /*
      we expect something like (order is random - at least from our point of view)
      dsl[0]  tango/device/1
      dsl[2]  TangoClassName1
      dsl[2]  dserver/executable_name/instance_name
      dsl[3]  DServer
      dsl[4]  tango/device/2
      dsl[5]  TangoClassName2
    */

    bool ds_found = false;

    std::string dserver_entry("dserver/" + ds);

    for ( tbfl::size_t i = 0; i < dsl.size(); i += 2 )
    {
      if ( ! ds_found && dsl[i] == dserver_entry )
      {
        ds_found = true;
        continue;
      }
      cl[dsl[i + 1]].push_back(dsl[i]);
      dl[dsl[i]] = dsl[i + 1];
      oc[dsl[i]] = dsl[i + 1];
    }

    if ( ! ds_found )
    {
      ostringstream oss;
      oss << "invalid device server name: there is no '"
          << ds
          << "' server defined into the Tango database";
      Tango::Except::throw_exception(_CPTC_("configuration error"),
                                     _CPTC_(oss.str().c_str()),
                                     _CPTC_("LvDeviceServer::get_devices_list"));
    }

    if ( dl.empty() )
    {
      ostringstream oss;
      oss << "invalid device server configuration: there is no Tango device attached to server "
          << ds
          << " [please check the Tango database]";
      Tango::Except::throw_exception(_CPTC_("configuration error"),
                                     _CPTC_(oss.str().c_str()),
                                     _CPTC_("LvDeviceServer::get_devices_list"));
    }

    //- avoid memory leak
    _BEST_EFFORT_TRY_(if ( ! tdb ) delete db);
  }
  catch ( ... )
  {
    //- avoid memory leak
    _BEST_EFFORT_TRY_(if ( ! tdb ) delete db);
    throw;
  }
}

//=============================================================================
// LvDeviceServer::parse_device_class_interfaces
//=============================================================================
void LvDeviceServer::parse_device_class_interfaces (LvDeviceServerCache& dsi)
    throw (Tango::DevFailed)
{
  //- clear the interface repository
  dsi.ds_class_interface.clear();

  //- build a tmp list of path to xmi files
  std::vector<std::string> xmi_file_path;
  std::string xmi_path_prop("PathToPogoXmiFile");
  LvClassPropertiesMapIterator cit = dsi.ds_class_properties.begin();
  for ( ; cit != dsi.ds_class_properties.end(); ++cit )
  {
    xmi_file_path.push_back(cit->second[xmi_path_prop][0] );
  }
  
  for( tbfl::size_t fp = 0; fp < xmi_file_path.size(); fp++ )
  {
    xmi::Classes cv;
    try
    {
      cv = xmi::Classes::from_file(xmi_file_path[fp]);
    }
    catch ( const std::runtime_error& rte )
    {
      std::ostringstream oss;
      oss << "parsing failed for .xmi file '"
          << xmi_file_path[fp]
          << "' ["
          << rte.what()
          << "]";
      Tango::Except::throw_exception(_CPTC_("xmi file parsing error"),
                                     _CPTC_(oss.str().c_str()),
                                     _CPTC_("LvDeviceServer::parse_device_class_interfaces"));
    }
    catch ( ... )
    {
      std::ostringstream oss;
      oss << "parsing failed for .xmi file '"
          << xmi_file_path[fp]
          << "' [unknown except caught]";
      Tango::Except::throw_exception(_CPTC_("xmi file parsing error"),
                                     _CPTC_(oss.str().c_str()),
                                     _CPTC_("LvDeviceServer::parse_device_class_interfaces"));
    }

    std::vector<Tango::CmdArgType> unsupported_data_types;
    unsupported_data_types.push_back(Tango::DEV_INT);
#if ! defined(_TBFL_HAS_DEV_ENCODED_SUPPORT_)
    unsupported_data_types.push_back(Tango::DEV_ENCODED);
#endif
#if ! defined(_TBFL_HAS_DEV_PIPE_SUPPORT_) || (TANGO_VERSION_MAJOR < 9)
    unsupported_data_types.push_back(Tango::DEV_PIPE_BLOB);
#endif
    //- check than store the class interface
    for ( tbfl::size_t i = 0; i < cv.classes.size(); i++ )
    {
      std::vector<std::string> bad_attrs;
      if ( cv.classes[i].has_attributes_by_type(unsupported_data_types, bad_attrs) )
      {
        ostringstream oss;
        oss << "the device class "
            << cv.classes[i].name
            << " contains the following unsupported attribute(s) [unsupported type]: ";
        for ( tbfl::size_t i = 0; i < bad_attrs.size(); i++ )
        {
          oss << bad_attrs[i] << ( i < bad_attrs.size() - 1 ? ", " : "" );
        }
        Tango::Except::throw_exception(_CPTC_("configuration error"),
                                       _CPTC_(oss.str().c_str()),
                                       _CPTC_("LvDeviceServer::parse_device_class_interfaces"));
      }
      //- ok, can store the class...
      dsi.ds_class_interface[cv.classes[i].name] = cv.classes[i];
    }
  }
}

//=============================================================================
// LvDeviceServer::start_devices
//=============================================================================
void LvDeviceServer::start_devices ()
    throw (Tango::DevFailed)
{
  TBFL_TRACE(this);
  
  //- be sure we have something to start!
  if ( device_server_cache_.ds_device_to_class.empty() )
  {
    Tango::Except::throw_exception(_CPTC_("invalid configuration"),
                                   _CPTC_("unexpected empty device list!"),
                                   _CPTC_("LvDeviceServer::start_devices"));
  }

  //- fully qualified device server name
  std::string ds_name(device_server_cache_.executable_name + "/" + device_server_cache_.instance_name);

  tbfl::uint32 source_id = 0xFFFFFFFF;
  
  LvServerDevicesMapIterator dit = device_server_cache_.ds_device_to_class.begin();
  for ( ; dit != device_server_cache_.ds_device_to_class.end(); ++dit )
  {
    //- instanciate or modify the device entry
    if ( ! tango_is_running_ )
    {
      TBFL_STDOUT("LvDeviceServer::start_devices:starting device "
                  <<  dit->first
                  << " from class "
                  <<  dit->second
                  << " with evt.ref.num "
                  << device_server_cache_.ds_event);
      //- create an entry in our device repository
      LvDevice * ldi = new LvDevice;
      ldi->ds_executable_name_ = device_server_cache_.executable_name;
      ldi->ds_instance_name_ = device_server_cache_.instance_name;
      ldi->class_name_ = dit->second;
      ldi->original_class_name_ = device_server_cache_.ds_device_to_original_class[dit->first];
      ldi->device_name_ = dit->first;
      ldi->lv_evt_ref_ = device_server_cache_.ds_event;
      ldi->lv_evt_source_id_ = source_id--; 
      ldi->class_interface_ = &device_server_cache_.ds_class_interface[device_server_cache_.ds_device_to_class[dit->first]];
      ldi->device_class_ = 0;
      ldi->device_impl_ = 0;
      devices_by_name_[dit->first] = ldi;
      devices_by_src_[ldi->lv_evt_source_id_] = ldi;      
      //- create a temporary null entry in our device class repository
      classes_by_names_[device_server_cache_.ds_device_to_class[dit->first]] = 0;
      //- register the device instance so that a device can retrieve its info from its ctor
      LV_DEV_INST_REP->register_device_instance(dit->first, ldi->lv_evt_source_id_, ldi);
    }
    else
    {
      TBFL_STDOUT("LvDeviceServer::restarting existing device "
                  << dit->first
                  << " from class "
                  << dit->second
                  << " [changing device server event ref. and interface]"
                  << "[evt id. is #"
                  << static_cast<tbfl::int32>(device_server_cache_.ds_event)
                  << "]");
      //- change evt. ref. num.
      devices_by_name_[dit->first]->lv_evt_ref_ = device_server_cache_.ds_event;
      //- change interface
      devices_by_name_[dit->first]->class_interface_ = &device_server_cache_.ds_class_interface[device_server_cache_.ds_device_to_class[dit->first]];
    }
  }

  if ( ! tango_is_running_  )
  {
    //- start the Tango kernel
    //- this will magically instanciate the devices using our class and device factories 
    LvTangoKernelThread::init();
    tango_is_running_ = true;
  }
  else
  {
    //- (re)start the device
    LvDeviceByNameMapIterator it = devices_by_name_.begin();
    for ( ; it != devices_by_name_.end(); ++it )
    {
      it->second->start();
    }
  }
}

//=============================================================================
// LvDeviceServer::stop_devices
//=============================================================================
void LvDeviceServer::stop_devices (bool about_to_exit)
    throw (Tango::DevFailed)
{
  TBFL_TRACE(this);

  LvDeviceByNameMapIterator it = devices_by_name_.begin();
  for ( ; it != devices_by_name_.end(); ++it )
  {
    if ( it->second )
    {
      it->second->stop();
    }
  }
}

//=============================================================================
// LvDeviceServer::log
//=============================================================================
void LvDeviceServer::log (const std::string& device_name, LoggingLevel level, const std::string& text)
{
  check_device_exists(device_name, TBFL_FUNC);

  devices_by_name_[device_name]->log(level, text);
}

//=============================================================================
// LvDeviceServer::set_client_request_timeout_by_class
//=============================================================================
void LvDeviceServer::set_client_request_timeout_by_class (const std::string& class_name, double tmo_in_sec)
{
  TBFL_TRACE(class_name);
  
  LvDeviceByNameMapIterator it = devices_by_name_.begin();
  for ( ; it != devices_by_name_.end(); ++it )
  {
    if ( it->second->class_name_ == class_name )
    {
       it->second->set_client_request_timeout(tmo_in_sec);
    }
  }
}

//=============================================================================
// LvDeviceServer::set_client_request_timeout_by_device
//=============================================================================
void LvDeviceServer::set_client_request_timeout_by_device (const std::string& device_name, double tmo_in_sec)
{
  TBFL_TRACE(device_name);

  check_device_exists(device_name, TBFL_FUNC);

  devices_by_name_[device_name]->set_client_request_timeout(tmo_in_sec);
}

//=============================================================================
// LvDeviceServer::enable_state_machine
//=============================================================================
void LvDeviceServer::enable_state_machine (const std::string& device_name, bool enable)
{
  TBFL_TRACE(device_name);

  check_device_exists(device_name, TBFL_FUNC);

  devices_by_name_[device_name]->enable_state_machine(enable);
}

//=============================================================================
// LvDeviceServer::class_factory
//=============================================================================
void LvDeviceServer::class_factory (Tango::DServer* ds)
{
  TBFL_TRACE(this);
  
  if ( ! ds )
  {
    //- Tango is written so that this should never happen
    return;
  }

  //- store Tango::DServer instance
  device_server_ = ds;
  
  LvServerDevicesMapConstIterator it = device_server_cache_.ds_device_to_class.begin();
  for( ; it != device_server_cache_.ds_device_to_class.end(); ++it )
  {
    TBFL_STDOUT("class_factory:instanciating Tango device class: " << it->first);
    LvDeviceClass * ldc = LvDeviceClass::init(it->second, it->first);
    classes_by_names_[it->first] = ldc;
    ds->_add_class(ldc);
  }
}

//=============================================================================
// LvDeviceServer::device_factory
//=============================================================================
LvDeviceImpl* LvDeviceServer::device_factory (LvDeviceClass* dc, const std::string& dn)
{
  LvDeviceByNameMapConstIterator it = devices_by_name_.find(dn);
  if ( it == devices_by_name_.end() )
  {
    std::ostringstream oss;
    oss << "unexpected device name [tango database vs labview registration mismatch]"
        << " [can't instanciate Tango device "
        << dn
        << "]";
    TBFL_STDOUT(oss.str().c_str());
    Tango::Except::throw_exception(_CPTC_("configuration error"),
                                   _CPTC_(oss.str().c_str()),
                                   _CPTC_("LvDeviceServer::device_factory"));
  }
  
  if ( classes_by_names_.find(dc->get_name()) == classes_by_names_.end() )
  {
    std::ostringstream oss;
    oss << "unexpected device class name [tango database vs labview registration mismatch]"
        << " [can't instanciate Tango device "
        << dn
        << " of unknown class "
        << dc->get_name()
        << "]";
    TBFL_STDOUT(oss.str().c_str());
    Tango::Except::throw_exception(_CPTC_("configuration error"),
                                   _CPTC_(oss.str().c_str()),
                                   _CPTC_("LvDeviceServer::device_factory"));
  }

  if ( device_server_cache_.ds_class_interface.find(dc->get_name()) ==  device_server_cache_.ds_class_interface.end() )
  {
    std::ostringstream oss;
    oss << "unknown class interface "
        << " [can't instanciate Tango device "
        << dn
        << " of unknown class "
        << dc->get_name()
        << " - class not found in any of the specified .xmi files]";
    TBFL_STDOUT(oss.str().c_str());
    Tango::Except::throw_exception(_CPTC_("configuration error"),
                                   _CPTC_(oss.str().c_str()),
                                   _CPTC_("LvDeviceServer::device_factory"));
  }

  LvDevice* ldi = it->second;
  ldi->device_class_ = dc;
  ldi->device_impl_ = new LvDeviceImpl(dc, dn);
  
  return ldi->device_impl_;
}

//=============================================================================
// LvDeviceServer::abort_client_requests
//=============================================================================
void LvDeviceServer::abort_client_requests ()
{
  LvDeviceByNameMapIterator it = devices_by_name_.begin();
  for(; it != devices_by_name_.end(); ++it )
  {
    if ( it->second && it->second->device_impl_ )
    {
      it->second->abort_client_requests();
    }
  }
}

//=============================================================================
// LvDeviceServer::set_execute_command_argout
//=============================================================================
void LvDeviceServer::set_execute_command_argout (LvExecuteCommandReplyPtr ecr)
    throw (Tango::DevFailed)
{
  LvDeviceBySourceMapIterator it = devices_by_src_.find(ecr->ack.source_id);
  if ( it == devices_by_src_.end() )
  {
    std::ostringstream oss;
    oss << "unknown event source [please, make sure that your VIs propagate the ClientRequest cluster without any modification]";
    Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                   _CPTC_(oss.str().c_str()),
                                   _CPTC_("LvDeviceServer::set_execute_command_argout"));
  }
  if ( ! it->second )
  {
    Tango::Except::throw_exception(_CPTC_("internal error"),
                                   _CPTC_("unexpected null pointer to device instance!"),
                                   _CPTC_("LvDeviceServer::set_execute_command_argout"));
  }

  //- notify the waiter...
  it->second->set_execute_command_argout(ecr);
}

//=============================================================================
// LvDeviceServer::set_read_attr_value
//=============================================================================
void LvDeviceServer::set_read_attribute_value (LvReadAttributeReplyPtr rar)
    throw (Tango::DevFailed)
{
  LvDeviceBySourceMapIterator it = devices_by_src_.find(rar->ack.source_id);
  if ( it == devices_by_src_.end() )
  {
    std::ostringstream oss;
    oss << "unknown event source [please, make sure that your VIs propagate the ClientRequest cluster without any modification]";
    Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                   _CPTC_(oss.str().c_str()),
                                   _CPTC_("LvDeviceServer::set_read_attr_value"));
  }
  if ( ! it->second )
  {
    Tango::Except::throw_exception(_CPTC_("internal error"),
                                   _CPTC_("unexpected null pointer to device instance!"),
                                   _CPTC_("LvDeviceServer::set_read_attr_value"));
  }

  //- notify the waiter...
  it->second->set_read_attribute_value(rar);
}

//=============================================================================
// LvDeviceServer::ack_client_request
//=============================================================================
void LvDeviceServer::ack_client_request (LvClientRequestReplyPtr cra)
    throw (Tango::DevFailed)
{
  //- identify request source
  LvDeviceBySourceMapIterator it = devices_by_src_.find(cra->source_id);
  if ( it == devices_by_src_.end() )
  {
    std::ostringstream oss;
    oss << "unknown event source [please, make sure that your VIs propagate the ClientRequest cluster without any modification]";
    Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                   _CPTC_(oss.str().c_str()),
                                   _CPTC_("LvDeviceServer::ack_client_request"));
  }
  
  if ( ! it->second )
  {
    Tango::Except::throw_exception(_CPTC_("internal error"),
                                   _CPTC_("unexpected null pointer to device instance"),
                                   _CPTC_("LvDeviceServer::ack_client_request"));
  }

  //- notify the waiter...
  it->second->ack_client_request(cra);
}

//=============================================================================
// LvDeviceServer::push_data_ready_event
//=============================================================================
void LvDeviceServer::push_data_ready_event (const std::string& dn, const std::string& an, tbfl::int32 uc)
    throw (Tango::DevFailed)
{
  TBFL_TRACE(dn + "/" + an);

  check_device_exists(dn, TBFL_FUNC);

  devices_by_name_[dn]->push_data_ready_event(an, uc);
}

//=============================================================================
// LvDeviceServer::get_device_property
//=============================================================================
void LvDeviceServer::get_device_property (const std::string& dn, const std::string&  pn, LvDevicePropertyPtr pv)
    throw (Tango::DevFailed)
{
  TBFL_TRACE(dn + "/" + pn);

  check_device_exists(dn, TBFL_FUNC);

  devices_by_name_[dn]->get_device_property(pn, pv);
}

//=============================================================================
// LvDeviceServer::get_attribute_info
//=============================================================================
void LvDeviceServer::get_attribute_info (const std::string& dn, const std::string& an, LvAttributeInfoPtr ai)
  throw (Tango::DevFailed)
{
  TBFL_TRACE (dn + "/" + an);

  check_device_exists (dn, TBFL_FUNC);

  devices_by_name_[dn]->get_attribute_info(an, ai);
}

//=============================================================================
// LvDeviceServer::get_attribute_info
//=============================================================================
void LvDeviceServer::set_attribute_info (const std::string& dn, const std::string& an, LvAttributeInfoPtr ai)
throw (Tango::DevFailed)
{
  TBFL_TRACE (dn + "/" + an);

  check_device_exists (dn, TBFL_FUNC);

  devices_by_name_[dn]->set_attribute_info(an, ai);
}

//=============================================================================
// LvDeviceServer::check_device_exists
//=============================================================================
void LvDeviceServer::check_device_exists (const std::string& dn, const std::string& loc)
    throw (Tango::DevFailed)
{
  LvDeviceByNameMapIterator it = devices_by_name_.find(dn);
  if ( it == devices_by_name_.end() )
  {
    std::ostringstream oss;
    oss << "invalid device name "
        << " [there is no Tango device named '"
        << dn
        << "' in the running LabVIEW device server]";
        
    TBFL_STDOUT(oss.str().c_str());

    Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                   oss.str().c_str(),
                                   loc.c_str());
  }
}

#endif //- _TBFL_HAS_DEVICE_SERVER_SUPPORT_
