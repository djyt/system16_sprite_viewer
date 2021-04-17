/***************************************************************************
    XML Configuration File Handling.

    Load Settings.

    Copyright Chris White.
    See license.txt for more details.
***************************************************************************/


#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <iostream>

#include "config.hpp"
#include "globals.hpp"


Config config;

Config::Config(void)
{

}


Config::~Config(void)
{
}


// Set Path to load and save config to
void Config::set_config_file(const std::string& file)
{
    cfg_file = file;
}

using boost::property_tree::ptree;
ptree pt_config;

bool Config::load()
{
    // Load XML file and put its contents in property tree. 
    // No namespace qualification is needed, because of Koenig 
    // lookup on the second argument. If reading fails, exception is thrown.
    try
    {
        read_xml(cfg_file, pt_config, boost::property_tree::xml_parser::trim_whitespace);
    }
    catch (std::exception &e)
    {
        std::cout << "Error loading XML file: " << e.what() << std::endl;
        return false;
    }

    sprites.format      = pt_config.get("sprites.format", (int)format::UNKNOWN);
    sprites.interleave  = pt_config.get("sprites.interleave", 0);
    sprites.swap_endian = pt_config.get("sprites.swap_endian", false);

    for (ptree::value_type &v : pt_config.get_child("sprites.roms"))
    {
        rom_t rom;
        rom.filename   = v.second.get_child("<xmlattr>.name").data();
        rom.dst_offset = std::stoi(v.second.get_child("<xmlattr>.offset").data(), nullptr, 0);
        rom.length     = std::stoi(v.second.get_child("<xmlattr>.length").data(), nullptr, 0);

        if (v.second.get_child_optional("<xmlattr>.continue"))
            rom.src_offset = std::stoi(v.second.get_child("<xmlattr>.continue").data(), nullptr, 0);

        sprites.rom.push_back(rom);
    }

    sprites.palname = pt_config.get("sprites.palette.pal.<xmlattr>.name", "");
    if (!sprites.palname.empty())
    {
        sprites.pal_bytes_per_entry = pt_config.get("sprites.palette.pal.<xmlattr>.bytes_per_entry", PAL_LENGTH);
        sprites.pal_offset          = pt_config.get("sprites.palette.pal.<xmlattr>.offset", 0);
    }

    return true;
}
