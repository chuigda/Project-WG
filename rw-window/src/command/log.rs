use std::fs::OpenOptions;
use tracing_subscriber::Registry;
use tracing_subscriber::layer::SubscriberExt;
use crate::config::RwConfig;
use crate::message_box;
use crate::opt::RwOptions;

pub fn setup_logger(config: Option<&RwConfig>, options: &RwOptions) -> bool {
    let log_file_name = config
        .and_then(|cfg| cfg.log.as_ref())
        .and_then(|log_cfg| Some(&log_cfg.file_name))
        .or(options.log_file.as_ref());
    if let Some(log_file_name) = log_file_name {
        let file_appender = OpenOptions::new()
            .write(true)
            .append(true)
            .create(true)
            .open(log_file_name);
        match file_appender {
            Ok(appender) => {
                let _ = tracing::subscriber::set_global_default(
                    Registry::default()
                        .with(tracing_subscriber::fmt::layer()
                            .with_ansi(false)
                            .with_writer(appender))
                        .with(tracing_subscriber::fmt::layer()
                            .with_writer(std::io::stdout))
                );
                true
            }
            Err(e) => {
                tracing_subscriber::fmt::init();
                tracing::warn!("无法打开日志文件: {e}，日志功能将不可用");
                message_box!("错误", &format!("无法打开日志文件，日志功能将不可用: \r\n{e}"));
                false
            }
        }
    } else {
        tracing_subscriber::fmt::init();
        false
    }
}
