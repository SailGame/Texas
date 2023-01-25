use crate::provider::client::{TexasProvider, TexasProviderConfig};

mod game;
mod provider;

use clap::Parser;
use tonic::transport::Uri;

/// Simple program to greet a person
#[derive(Parser, Debug)]
#[command(author, version, about, long_about = None)]
struct Args {
    #[arg(short, long)]
    id: String,

    #[arg(short, long)]
    core_url: String,
}

#[tokio::main(flavor = "current_thread")]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let args = Args::parse();

    let uri = args.core_url.parse::<Uri>().unwrap();

    let config = TexasProviderConfig{
        m_id: args.id.clone(),
        m_core_address: uri,
    };

    let mut provider = TexasProvider::new(config).await;
    provider.poll().await;
    Ok(())
}
