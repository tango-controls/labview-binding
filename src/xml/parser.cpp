// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   DataAdapter.cpp 
//
// = AUTHOR
//   Julien Berthault - SOLEIL
//
// ============================================================================

//-----------------------------------------------------------------------------
// DEPENDENCIES
//-----------------------------------------------------------------------------
#include "Config.h"

#if defined (_TBFL_HAS_DEVICE_SERVER_SUPPORT_)

#include <sstream>
#include <cassert>
#include <algorithm>
#include <iterator>

#include "xml/parser.h"
#include "xml/tinyxml2.h"

namespace xmi {

namespace converters {

#define _UNSPECIFIED_VALUE_ ""

template<typename T>
T as(const std::string& value) {
    T result;
    std::istringstream iss(value);
    if (!(iss >> result))
        throw std::runtime_error("wrong value \"" + value + '"');
    return result;
}

double to_double(const std::string& value) {
    return as<double>(value);
}

int to_int(const std::string& value) {
    return value.empty() ? 0 : as<int>(value);
}

bool to_bool(const std::string& value) {
    if (value == "true") return true;
    if (value == "false") return false;
    throw std::runtime_error("wrong boolean value \"" + value + '"');
}

TriState to_tristate(const std::string& value) {
    if (value == "true") return TRUE_STATE;
    if (value == "false") return FALSE_STATE;
    if (value == "N/A") return UNDEFINED_STATE;
    throw std::runtime_error("wrong tristate value \"" + value + '"');
}
    
Tango::CmdArgType to_devtype(const std::string& value) {
    if (value == "pogoDsl:VoidType") return Tango::DEV_VOID; 
    if (value == "pogoDsl:BooleanType") return Tango::DEV_BOOLEAN;
    if (value == "pogoDsl:UCharType") return Tango::DEV_UCHAR;
    if (value == "pogoDsl:ShortType") return Tango::DEV_SHORT;
    if (value == "pogoDsl:UShortType") return Tango::DEV_USHORT;
    if (value == "pogoDsl:IntType") return Tango::DEV_LONG;
    if (value == "pogoDsl:UIntType") return Tango::DEV_ULONG;
    if (value == "pogoDsl:LongType") return Tango::DEV_LONG64;
    if (value == "pogoDsl:ULongType") return Tango::DEV_ULONG64;
    if (value == "pogoDsl:FloatType") return Tango::DEV_FLOAT;
    if (value == "pogoDsl:DoubleType") return Tango::DEV_DOUBLE;
    if (value == "pogoDsl:StringType") return Tango::DEV_STRING;
    if (value == "pogoDsl:CharArrayType") return Tango::DEVVAR_CHARARRAY;
    if (value == "pogoDsl:BooleanArrayType") return Tango::DEVVAR_BOOLEANARRAY;
    if (value == "pogoDsl:ShortArrayType") return Tango::DEVVAR_SHORTARRAY;
    if (value == "pogoDsl:ShortVectorType") return Tango::DEVVAR_SHORTARRAY;
    if (value == "pogoDsl:UShortArrayType") return Tango::DEVVAR_USHORTARRAY;
    if (value == "pogoDsl:UShortVectorType") return Tango::DEVVAR_USHORTARRAY;
    if (value == "pogoDsl:IntArrayType") return Tango::DEVVAR_LONGARRAY;
    if (value == "pogoDsl:IntVectorType") return Tango::DEVVAR_LONGARRAY;
    if (value == "pogoDsl:UIntArrayType") return Tango::DEVVAR_ULONGARRAY;
    if (value == "pogoDsl:UIntVectorType") return Tango::DEVVAR_ULONGARRAY;
    if (value == "pogoDsl:LongArrayType") return Tango::DEVVAR_LONG64ARRAY;
    if (value == "pogoDsl:ULongArrayType") return Tango::DEVVAR_ULONG64ARRAY;
    if (value == "pogoDsl:FloatArrayType") return Tango::DEVVAR_FLOATARRAY;
    if (value == "pogoDsl:FloatVectorType") return Tango::DEVVAR_FLOATARRAY;
    if (value == "pogoDsl:DoubleArrayType") return Tango::DEVVAR_DOUBLEARRAY;
    if (value == "pogoDsl:DoubleVectorType") return Tango::DEVVAR_DOUBLEARRAY;
    if (value == "pogoDsl:StringArrayType") return Tango::DEVVAR_STRINGARRAY;
    if (value == "pogoDsl:StringVectorType") return Tango::DEVVAR_STRINGARRAY;
    if (value == "pogoDsl:LongStringArrayType") return Tango::DEVVAR_LONGSTRINGARRAY;
    if (value == "pogoDsl:DoubleStringArrayType") return Tango::DEVVAR_DOUBLESTRINGARRAY;
    if (value == "pogoDsl:StateType") return Tango::DEV_STATE;
  	if (value == "pogoDsl:StateArrayType") return Tango::DEVVAR_STATEARRAY;
    if (value == "pogoDsl:ConstStringType") return Tango::CONST_DEV_STRING;
    if (value == "pogoDsl:EncodedType") return Tango::DEV_ENCODED;
   	if (value == "pogoDsl:EnumType") return Tango::DEV_ENUM;
    throw std::runtime_error("wrong type value \"" + value + '"');
}
 
Tango::DevState to_devstate(const std::string& value) {
    if (value == "ON") return Tango::ON;
    if (value == "OFF") return Tango::OFF;
    if (value == "CLOSE") return Tango::CLOSE;
    if (value == "OPEN") return Tango::OPEN;
    if (value == "INSERT") return Tango::INSERT;
    if (value == "EXTRACT") return Tango::EXTRACT;
    if (value == "MOVING") return Tango::MOVING;
    if (value == "STANDBY") return Tango::STANDBY;
    if (value == "FAULT") return Tango::FAULT;
    if (value == "INIT") return Tango::INIT;
    if (value == "RUNNING") return Tango::RUNNING;
    if (value == "ALARM") return Tango::ALARM;
    if (value == "DISABLE") return Tango::DISABLE;
    if (value == "UNKNOWN") return Tango::UNKNOWN;
    throw std::runtime_error("wrong state value \"" + value + '"');
}

Tango::EventType to_event(const std::string& value) {
    /// @todo check strings correctness !
    if (value == "changeEvent") return Tango::CHANGE_EVENT;
    if (value == "periodicEvent") return Tango::PERIODIC_EVENT;
    if (value == "archiveEvent") return Tango::ARCHIVE_EVENT;
    if (value == "userEvent") return Tango::USER_EVENT;
    if (value == "attrConfEvent") return Tango::ATTR_CONF_EVENT;
    if (value == "dataReadyEvent") return Tango::DATA_READY_EVENT;
    throw std::runtime_error("wrong event-type value \"" + value + '"');
}

Tango::DispLevel to_level(const std::string& value) {
    if (value == "OPERATOR") return Tango::OPERATOR;
    if (value == "EXPERT") return Tango::EXPERT;
    throw std::runtime_error("wrong level value \"" + value + '"');
}

Tango::AttrDataFormat to_format(const std::string& value) {
    if (value == "Scalar") return Tango::SCALAR;
    if (value == "Spectrum") return Tango::SPECTRUM;
    if (value == "Image") return Tango::IMAGE;
    throw std::runtime_error("wrong format value \"" + value + '"');
}

Tango::AttrWriteType to_rw(const std::string& value) {
    if (value == "READ") return Tango::READ;
    if (value == "READ_WITH_WRITE") return Tango::READ_WITH_WRITE;
    if (value == "WRITE") return Tango::WRITE;
    if (value == "READ_WRITE") return Tango::READ_WRITE;
    throw std::runtime_error("wrong RW value \"" + value + '"');
}

} // namespace converters

namespace parsers {

std::string find_attribute(const tinyxml2::XMLElement* element, const std::string& name) {
    const tinyxml2::XMLAttribute* attr = element->FindAttribute(name.c_str());
    if (!attr)
        throw std::runtime_error("missing attribute named \"" + name + '"');
    return attr->Value();
}

const tinyxml2::XMLElement* find_element(const tinyxml2::XMLElement* element, const std::string& name) {
    const tinyxml2::XMLElement* el = element->FirstChildElement(name.c_str());
    if (!el)
        throw std::runtime_error("missing element named \"" + name + '"');
    return el;
}

std::string find_attribute(const tinyxml2::XMLElement* element, const std::string& name, const std::string& default_value) {
    const tinyxml2::XMLAttribute* attr = element->FindAttribute(name.c_str());
    return attr ? attr->Value() : default_value;
}

std::string find_type(const tinyxml2::XMLElement* element, const std::string& name) {
    return find_attribute(find_element(element, name), "xsi:type");
}

struct sibling_iterator : public std::iterator<std::forward_iterator_tag, const tinyxml2::XMLElement*> {

    sibling_iterator() : element(NULL), name() {
    }

    sibling_iterator(const tinyxml2::XMLElement* root, const std::string& name) : element(NULL), name(name) {
        element = root->FirstChildElement(name.c_str());
    }

    sibling_iterator& operator++() { element = element->NextSiblingElement(name.c_str()); return *this; }
    sibling_iterator operator++(int) { sibling_iterator it(*this); operator ++(); return it; }

    bool operator ==(const sibling_iterator& rhs) const { return element == rhs.element; }
    bool operator !=(const sibling_iterator& rhs) const { return element != rhs.element; }

    const tinyxml2::XMLElement* operator*() const { return element; }

    const tinyxml2::XMLElement* element;
    std::string name;

};

void validate(const tinyxml2::XMLElement* element) {
    if (!element)
        throw std::runtime_error("got null element");
}

void validate(const tinyxml2::XMLElement* element, const std::string& name) {
    if (!element)
        throw std::runtime_error("got null element, expected \"" + name + '"');
    if (element->Name() != name)
        throw std::runtime_error("got unexpected tag");
}

void validate(const tinyxml2::XMLElement* element, const std::string& name1, const std::string& name2) {
    if (!element)
        throw std::runtime_error("got null element, expected \"" + name1 + "\" or \"" + name2 + '"');
    if (element->Name() != name1 && element->Name() != name2)
        throw std::runtime_error("got unexpected tag");
}

void validate(const tinyxml2::XMLElement* element, const std::string& name1, const std::string& name2, const std::string& name3) {
    if (!element)
        throw std::runtime_error("got null element, expected \"" + name1 + "\" or \"" + name2 + '"' + "\" or \"" + name3 + '"');
    if (element->Name() != name1 && element->Name() != name2  && element->Name() != name3)
        throw std::runtime_error("got unexpected tag");
}

#define XMI_VALIDATE(...) do { validate(__VA_ARGS__); } while (0)

std::string parse_default_value(const tinyxml2::XMLElement* element) {
  XMI_VALIDATE(element, "DefaultPropValue");
  return element->GetText();
}

Status parse_status(const tinyxml2::XMLElement* element) {
    XMI_VALIDATE(element, "status");
    Status result;
    result.abstract = converters::to_bool(find_attribute(element, "abstract"));
    result.inherited = converters::to_bool(find_attribute(element, "inherited"));
    result.concrete = converters::to_bool(find_attribute(element, "concrete"));
    result.concrete_here = converters::to_bool(find_attribute(element, "concreteHere", "false"));
    return result;
}

Argument parse_argument(const tinyxml2::XMLElement* element) {
    XMI_VALIDATE(element, "argin", "argout");
    Argument result;
    result.description = find_attribute(element, "description");
    result.type = converters::to_devtype(find_type(element, "type"));
    return result;
}

State parse_state(const tinyxml2::XMLElement* element) {
    XMI_VALIDATE(element, "states");
    State result;
    result.name = find_attribute(element, "name");
    result.description = find_attribute(element, "description");
    result.status = parse_status(element->FirstChildElement("status"));
    return result;
}

EventCriteria parse_event_criteria(const tinyxml2::XMLElement* element) {
    XMI_VALIDATE(element);
    EventCriteria result;
    result.rel_change = find_attribute(element, "relChange");
    result.abs_change = find_attribute(element, "absChange");
    result.period = find_attribute(element, "period");
    return result;
}

Event parse_event(const tinyxml2::XMLElement* element) {
    XMI_VALIDATE(element);
    Event result;
    result.type = converters::to_event(element->Name());
    result.fire = converters::to_bool(find_attribute(element, "fire"));
    result.check_criteria = converters::to_bool(find_attribute(element, "libCheckCriteria"));
    return result;
}

Property parse_property(const tinyxml2::XMLElement* element) {
    XMI_VALIDATE(element, "deviceProperties");
    Property result;
    result.name = find_attribute(element, "name");
    result.description = find_attribute(element, "description");
    result.type = converters::to_devtype(find_type(element, "type"));
    result.status = parse_status(element->FirstChildElement("status"));
    const tinyxml2::XMLElement* dv = element->FirstChildElement("DefaultPropValue");
    if (dv)
      result.default_value = parse_default_value(dv);
    return result;
}

Tango::DevState parse_devstate(const tinyxml2::XMLElement* element) {
    XMI_VALIDATE(element, "excludedStates", "readExcludedStates", "writeExcludedStates");
    return converters::to_devstate(element->GetText());
}

std::string parse_enum_labels(const tinyxml2::XMLElement* element) {
  XMI_VALIDATE(element, "enumLabels");
  return element->GetText();
}

Command parse_command(const tinyxml2::XMLElement* element) {
    XMI_VALIDATE(element, "commands");
    Command result;
    result.name = find_attribute(element, "name");
    result.description = find_attribute(element, "description");
    result.exec_method = find_attribute(element, "execMethod");
    result.display_level = converters::to_level(find_attribute(element, "displayLevel"));
    result.polling_period = converters::to_int(find_attribute(element, "polledPeriod"));
    result.argin = parse_argument(element->FirstChildElement("argin"));
    result.argout = parse_argument(element->FirstChildElement("argout"));
    result.status = parse_status(element->FirstChildElement("status"));
    std::transform(sibling_iterator(element, "excludedStates"), sibling_iterator(),
                   std::back_inserter(result.excluded_states), parse_devstate);
    return result;
}

void seek_event(const tinyxml2::XMLElement* root, const std::string& name, const std::string& criteria_name, std::vector<Event>& output) {
    XMI_VALIDATE(root);
    const tinyxml2::XMLElement* element = root->FirstChildElement(name.c_str());
    const tinyxml2::XMLElement* criteria_el = root->FirstChildElement(criteria_name.c_str());
    if (element) {
        Event event = parse_event(element);
        event.has_criteria = criteria_el != NULL;
        if (criteria_el)
            event.criteria = parse_event_criteria(criteria_el);
        output.push_back(event);
    }
}

Attribute parse_attribute(const tinyxml2::XMLElement* element) {
    XMI_VALIDATE(element, "attributes");
    Attribute result;
    result.name = find_attribute(element, "name");
    result.att_type = converters::to_format(find_attribute(element, "attType"));
    result.rw_type = converters::to_rw(find_attribute(element, "rwType"));
    result.display_level = converters::to_level(find_attribute(element, "displayLevel"));
    result.polling_period = converters::to_int(find_attribute(element, "polledPeriod"));
    result.max_x = converters::to_int(find_attribute(element, "maxX"));
    result.max_y = converters::to_int(find_attribute(element, "maxY"));
    result.memorized = converters::to_bool(find_attribute(element, "memorized", "false"));
    result.memorized_at_init = converters::to_bool(find_attribute(element, "memorizedAtInit", "false"));
    result.alloc_read_member = converters::to_tristate(find_attribute(element, "allocReadMember", "N/A"));
    result.is_dynamic = converters::to_tristate(find_attribute(element, "isDynamic", "N/A"));
    result.data_type = converters::to_devtype(find_type(element, "dataType"));
    seek_event(element, "changeEvent", "eventCriteria", result.events);
    seek_event(element, "periodicEvent", "evPeriodicEvent", result.events);
    seek_event(element, "archiveEvent", "evArchiveCriteria", result.events);
    seek_event(element, "userEvent", "evUserEvent", result.events);
    seek_event(element, "attrConfEvent", "evAttrConfEvent", result.events);
    seek_event(element, "dataReadyEvent", "evDataReadyEvent", result.events);
    result.status = parse_status(element->FirstChildElement("status"));
    const tinyxml2::XMLElement* properties_el = find_element(element, "properties");
    result.description = find_attribute(properties_el, "description", _UNSPECIFIED_VALUE_);
    result.label = find_attribute(properties_el, "label", _UNSPECIFIED_VALUE_);
    result.unit = find_attribute(properties_el, "unit", _UNSPECIFIED_VALUE_);
    result.standard_unit = find_attribute(properties_el, "standardUnit", _UNSPECIFIED_VALUE_);
    result.display_unit = find_attribute(properties_el, "displayUnit", _UNSPECIFIED_VALUE_);
    result.format = find_attribute(properties_el, "format", _UNSPECIFIED_VALUE_);
    result.min_value = find_attribute(properties_el, "minValue", _UNSPECIFIED_VALUE_);
    result.max_value = find_attribute(properties_el, "maxValue", _UNSPECIFIED_VALUE_);
    result.min_alarm = find_attribute(properties_el, "minAlarm", _UNSPECIFIED_VALUE_);
    result.max_alarm = find_attribute(properties_el, "maxAlarm", _UNSPECIFIED_VALUE_);
    result.min_warning = find_attribute(properties_el, "minWarning", _UNSPECIFIED_VALUE_);
    result.max_warning = find_attribute(properties_el, "maxWarning", _UNSPECIFIED_VALUE_);
    result.delta_time = find_attribute(properties_el, "deltaTime", _UNSPECIFIED_VALUE_);
    result.delta_value = find_attribute(properties_el, "deltaValue", _UNSPECIFIED_VALUE_);
    std::transform(sibling_iterator(element, "readExcludedStates"), sibling_iterator(),
                   std::back_inserter(result.read_excluded_states), parse_devstate);
    std::transform(sibling_iterator(element, "writeExcludedStates"), sibling_iterator(),
                   std::back_inserter(result.write_excluded_states), parse_devstate);
    std::transform(sibling_iterator(element, "enumLabels"), sibling_iterator(),
                   std::back_inserter(result.enum_labels), parse_enum_labels);
    return result;
}

Class parse_class(const tinyxml2::XMLElement* element) {
    XMI_VALIDATE(element, "classes");
    Class result;
    result.name = find_attribute(element, "name");
    result.description = find_attribute(find_element(element, "description"), "description");
    std::transform(sibling_iterator(element, "deviceProperties"), sibling_iterator(), std::back_inserter(result.device_properties), parse_property);
    // std::transform(sibling_iterator(element, "classProperties"), sibling_iterator(), std::back_inserter(result.class_properties), parse_property);
    std::transform(sibling_iterator(element, "commands"), sibling_iterator(), std::back_inserter(result.commands), parse_command);
    std::transform(sibling_iterator(element, "attributes"), sibling_iterator(), std::back_inserter(result.attributes), parse_attribute);
    std::transform(sibling_iterator(element, "states"), sibling_iterator(), std::back_inserter(result.states), parse_state);
    return result;
}

Classes parse_classes(const tinyxml2::XMLElement* element) {
    XMI_VALIDATE(element);
    Classes result;
    std::transform(sibling_iterator(element, "classes"), sibling_iterator(), std::back_inserter(result.classes), parse_class);
    return result;
}

} // namespace parsers

bool Attribute::property_specified (const std::string& property)
{
  return ! property.empty();
}

bool Class::has_attributes_by_type (Tango::CmdArgType t, std::vector<std::string>& bad_attrs)
{
  for ( std::size_t i = 0; i < attributes.size(); i++ )
  {
    if ( attributes[i].data_type == t )
    {
      bad_attrs.push_back(attributes[i].name);
      return true;
    }
  }
  return false;
}

bool Class::has_attributes_by_type (const std::vector<Tango::CmdArgType>& t, std::vector<std::string>& bad_attrs)
{
  for ( std::size_t i = 0; i < t.size(); i++ )
  {
    if ( has_attributes_by_type(t[i], bad_attrs) )
      return true;
  }
  return false;
}

bool Class::has_attributes_by_format (Tango::AttrDataFormat f, std::vector<std::string>& bad_attrs)
{
  for ( std::size_t i = 0; i < attributes.size(); i++ )
  {
    if ( attributes[i].att_type == f )
    {
      bad_attrs.push_back(attributes[i].name);
      return true;
    }
  }
  return false;
}

bool Class::has_attributes_by_format (const std::vector<Tango::AttrDataFormat>& f, std::vector<std::string>& bad_attrs)
{
  for ( std::size_t i = 0; i < f.size(); i++ )
  {
    if ( has_attributes_by_format(f[i], bad_attrs) )
      return true;
  }
  return false;
}

Tango::CmdArgType Class::device_property_data_type (const std::string& pn) const {
  for ( std::size_t i = 0; i < device_properties.size(); i++ )
  {
    if ( pn == device_properties[i].name )
      return device_properties[i].type;
  }
  throw std::runtime_error("no such device property");
}

Classes Classes::from_file(const std::string& filename) {
    tinyxml2::XMLDocument doc;
    doc.LoadFile(filename.c_str());
    if (doc.Error())
        throw std::runtime_error(doc.ErrorName());
    return parsers::parse_classes(doc.RootElement());
}

} // namespace xmi

template<typename It>
void write_vector(std::ostream& os, It first, It last) {
    os << "[";
    if (first != last) {
        os << *first;
        for (++first ; first != last ; ++first)
            os << ", " << *first;
    }
    os << "]";
}

std::ostream& operator << (std::ostream& os, const xmi::Property& property) {
    return os << "Property(" << property.name << ")";
}

std::ostream& operator << (std::ostream& os, const xmi::Command& command) {
    return os << "Command(" << command.name << ")";
}

std::ostream& operator << (std::ostream& os, const xmi::Attribute& attribute) {
    return os << "Attribute(" << attribute.name << ")";
}

std::ostream& operator << (std::ostream& os, const xmi::Class& class_) {
    os << "Class(" << class_.name << ", ";
    write_vector(os, class_.device_properties.begin(), class_.device_properties.end());
    write_vector(os, class_.class_properties.begin(), class_.class_properties.end());
    write_vector(os, class_.commands.begin(), class_.commands.end());
    write_vector(os, class_.attributes.begin(), class_.attributes.end());
    return os << ")";
}

std::ostream& operator << (std::ostream& os, const xmi::Classes& classes) {
    os << "Classes(";
    write_vector(os, classes.classes.begin(), classes.classes.end());
    return os << ")";
}

#endif //- _TBFL_HAS_DEVICE_SERVER_SUPPORT_

