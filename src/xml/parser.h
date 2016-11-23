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

#pragma once

//-----------------------------------------------------------------------------
// DEPENDENCIES
//-----------------------------------------------------------------------------
#include "Config.h"

#if defined (_TBFL_HAS_DEVICE_SERVER_SUPPORT_)

#include <string>
#include <ostream>
#include <vector>

#include <tango.h>

namespace xmi {

enum TriState {
    FALSE_STATE,
    TRUE_STATE,
    UNDEFINED_STATE
};

struct Argument {
    std::string description;
    Tango::CmdArgType type;
};

struct Status {
    bool abstract;
    bool inherited;
    bool concrete;
    bool concrete_here;
};

struct Property {
    std::string name;
    std::string description;
    Tango::CmdArgType type;
    string default_value;
    Status status;
};

struct State {
    std::string name;
    std::string description;
    Status status;
};

struct Command {
    std::string name;
    std::string description;
    std::string exec_method;
    Tango::DispLevel display_level;
    int polling_period; /// @note int vs double ?
    Argument argin;
    Argument argout;
    Status status;
    std::vector<Tango::DevState> excluded_states;
};

struct EventCriteria {
    std::string rel_change;
    std::string abs_change;
    std::string period;
};

struct Event {
    Tango::EventType type;
    bool fire;
    bool check_criteria;
    bool has_criteria;
    EventCriteria criteria; /// if not has_criteria, this value is dummy
};

struct Attribute {
    std::string name;
    Tango::AttrDataFormat att_type;
    Tango::AttrWriteType rw_type;
    Tango::DispLevel display_level;
    int polling_period; /// @note int vs double vs size_t ?
    int max_x;
    int max_y;
    bool memorized;
    bool memorized_at_init;
    TriState alloc_read_member;
    TriState is_dynamic;
    Tango::CmdArgType data_type;
    std::vector<Event> events;
    Status status;
    std::string description;
    std::string label;
    std::string unit;
    std::string standard_unit;
    std::string display_unit;
    std::string format;
    std::string min_value;
    std::string max_value;
    std::string min_alarm;
    std::string max_alarm;
    std::string min_warning;
    std::string max_warning;
    std::string delta_time; /// @note string vs int vs double vs size_t ?
    std::string delta_value;
    std::vector<Tango::DevState> read_excluded_states;
    std::vector<Tango::DevState> write_excluded_states;
    std::vector<std::string> enum_labels;
    static bool property_specified (const std::string& property);
};

struct Class {
    std::string name;
    std::string description;
    std::vector<Property> device_properties;
    std::vector<Property> class_properties;
    std::vector<Command> commands;
    std::vector<Attribute> attributes;
    std::vector<State> states;
    bool has_attributes_by_type (Tango::CmdArgType t, std::vector<std::string>& bad_attrs);
    bool has_attributes_by_type (const std::vector<Tango::CmdArgType>& t, std::vector<std::string>& bad_attrs);
    bool has_attributes_by_format (Tango::AttrDataFormat f, std::vector<std::string>& bad_attrs);
    bool has_attributes_by_format (const std::vector<Tango::AttrDataFormat>& fn, std::vector<std::string>& bad_attrs);
    Tango::CmdArgType device_property_data_type (const std::string& pn) const; 
};

struct Classes {

    static Classes from_file(const string &filename);

    std::vector<Class> classes;
};

} // namespace xmi

std::ostream& operator << (std::ostream& os, const xmi::Property& property);
std::ostream& operator << (std::ostream& os, const xmi::Command& command);
std::ostream& operator << (std::ostream& os, const xmi::Attribute& attribute);
std::ostream& operator << (std::ostream& os, const xmi::Class& class_);
std::ostream& operator << (std::ostream& os, const xmi::Classes& classes);

#endif //- _TBFL_HAS_DEVICE_SERVER_SUPPORT_
