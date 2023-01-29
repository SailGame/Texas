fn main() -> Result<(), Box<dyn std::error::Error>> {
    tonic_build::compile_protos("externals/proto/texas/texas.proto")?;
    tonic_build::configure()
        .build_client(true)
        .compile(
            &["externals/proto/core/core.proto"],
            &["externals/proto"],
        )?;
    Ok(())
}