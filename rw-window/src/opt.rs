use structopt::StructOpt;

#[derive(Debug, StructOpt)]
#[structopt(name = "rw-window", about = "Main executable of the Rusty Project-WG")]
pub struct RwOptions {
    #[structopt(long = "config-file")]
    pub config_file: Option<String>,

    #[structopt(long = "list-vk-devices")]
    pub list_devices: bool,

    #[structopt(short = "b", long = "bind-addr")]
    pub bind_addr: Option<String>,

    #[structopt(short = "p", long = "bind-port")]
    pub bind_port: Option<u16>,

    #[structopt(short = "l", long = "log")]
    pub log_file: Option<String>,

    #[structopt(long = "vk-device-id")]
    pub vulkan_device_id: Option<String>
}
