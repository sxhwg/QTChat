#pragma once
#include <iostream>
#include <map>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/filesystem.hpp>

struct SectionInfo
{
	SectionInfo();
	~SectionInfo();
	SectionInfo(const SectionInfo &src);
	SectionInfo &operator=(const SectionInfo &src);
	std::string operator[](const std::string &key);

	std::string GetValue(const std::string &key);

	std::map<std::string, std::string> m_section_datas;
};

/* 配置文件管理类，用于处理配置文件和获取配置信息 */
class ConfigMgr
{
public:
	~ConfigMgr();
	ConfigMgr(const ConfigMgr &src);
	ConfigMgr &operator=(const ConfigMgr &src);
	SectionInfo operator[](const std::string &section);

	static ConfigMgr &Inst();
	std::string GetValue(const std::string &section, const std::string &key);

private:
	ConfigMgr();
	std::map<std::string, SectionInfo> m_config_map;
};