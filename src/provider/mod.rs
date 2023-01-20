mod texas_proto {
    tonic::include_proto!("texas");
}

mod core_proto {
    tonic::include_proto!("core");
}

mod logger;
pub mod client;