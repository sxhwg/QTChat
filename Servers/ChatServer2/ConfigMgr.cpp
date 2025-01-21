#include "ConfigMgr.h"

SectionInfo::SectionInfo()
{
}

SectionInfo::~SectionInfo()
{
	m_section_datas.clear();
}

SectionInfo::SectionInfo(const SectionInfo &src)
{
	m_section_datas = src.m_section_datas;
}

SectionInfo &SectionInfo::operator=(const SectionInfo &src)
{
	if (&src == this)
	{
		return *this;
	}

	this->m_section_datas = src.m_section_datas;
	return *this;
}

std::string SectionInfo::operator[](const std::string &key)
{
	if (m_section_datas.find(key) == m_section_datas.end())
	{
		return "";
	}
	return m_section_datas[key];
}

std::string SectionInfo::GetValue(const std::string &key)
{
	if (m_section_datas.find(key) == m_section_datas.end())
	{
		return "";
	}
	return m_section_datas[key];
}

ConfigMgr::~ConfigMgr()
{
	m_config_map.clear();
}

ConfigMgr::ConfigMgr(const ConfigMgr &src)
{
	m_config_map = src.m_config_map;
}

ConfigMgr &ConfigMgr::operator=(const ConfigMgr &src)
{
	if (&src == this)
	{
		return *this;
	}

	m_config_map = src.m_config_map;
	return *this;
}

SectionInfo ConfigMgr::operator[](const std::string &section)
{
	if (m_config_map.find(section) == m_config_map.end())
	{
		return SectionInfo();
	}
	return m_config_map[section];
}

ConfigMgr &ConfigMgr::Inst()
{
	static ConfigMgr c_mgr;
	return c_mgr;
}

std::string ConfigMgr::GetValue(const std::string &section, const std::string &key)
{
	if (m_config_map.find(section) == m_config_map.end())
	{
		return "";
	}

	return m_config_map[section].GetValue(key);
}

ConfigMgr::ConfigMgr()
{
	// 获取当前工作目录
	boost::filesystem::path current_path = boost::filesystem::current_path();
	// 构建config.ini文件的完整路径
	boost::filesystem::path config_path = current_path / "config.ini";
	std::cout << "Config path: " << config_path << std::endl;

	// 使用Boost.PropertyTree来读取INI文件
	boost::property_tree::ptree pt;
	boost::property_tree::read_ini(config_path.string(), pt);

	// 遍历INI文件中的所有section
	for (const auto &section_pair : pt)
	{
		const std::string &section_name = section_pair.first;
		const boost::property_tree::ptree &section_tree = section_pair.second;

		// 对于每个section，遍历其所有的key-value对
		std::map<std::string, std::string> section_config;
		for (const auto &key_value_pair : section_tree)
		{
			const std::string &key = key_value_pair.first;
			const std::string &value = key_value_pair.second.get_value<std::string>();
			section_config[key] = value;
		}
		SectionInfo sectionInfo;
		sectionInfo.m_section_datas = section_config;
		// 将section的key-value对保存到config_map中
		m_config_map[section_name] = sectionInfo;
	}

	// 输出所有的section和key-value对
	for (const auto &section_entry : m_config_map)
	{
		const std::string &section_name = section_entry.first;
		SectionInfo section_config = section_entry.second;
		std::cout << "[" << section_name << "]" << std::endl;
		for (const auto &key_value_pair : section_config.m_section_datas)
		{
			std::cout << key_value_pair.first << "=" << key_value_pair.second << std::endl;
		}
	}
}
