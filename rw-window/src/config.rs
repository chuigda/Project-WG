use std::fs::read_to_string;
use serde::{Serialize, Deserialize};

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(rename_all = "kebab-case")]
pub struct RwConfig {
    pub vulkan: Option<RwVulkanConfig>,
    pub protocol: Option<RwProtocolConfig>,
    pub log: Option<RwLogConfig>
}

impl Default for RwConfig {
    fn default() -> Self {
        Self {
            vulkan: None,
            protocol: Some(RwProtocolConfig::default()),
            log: None
        }
    }
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(rename_all = "kebab-case")]
pub struct RwProtocolConfig {
    pub ip_addr: String,
    pub port: u16
}

impl Default for RwProtocolConfig {
    fn default() -> Self {
        Self {
            ip_addr: "127.0.0.1".into(),
            port: 3201
        }
    }
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(rename_all = "kebab-case")]
pub struct RwLogConfig {
    pub file_name: String,
    pub log_level: String
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(rename_all = "kebab-case")]
pub struct RwVulkanConfig {
    pub device_id: Option<u32>,
    pub device_index: Option<usize>
}

pub fn try_read_config(file_name: &str) -> Option<RwConfig> {
    let file_content: String = read_to_string(file_name).ok()?;
    toml::from_str::<RwConfig>(&file_content).ok()
}
