# Hyperledger Fabric Playground

This project provides several useful Docker-Compose script to help quickly bootup a Hyperledger Fabric network, and do simple testing with deploy, invoke and query transactions.

Currently we support Hyperledger Fabric v1.0.5.


## Supported Fabric Releases

Fabric Release | Description
--- | ---
[Fabric v1.0.5](v1.0.5/)


## Getting Started

Enter the subdir of specific version, e.g., 

```bash
$ cd v105/
```

If running on a containerOS like coreOS install make like so:

```bash
$ docker run -ti --rm -v /opt/bin:/out ubuntu:14.04   /bin/bash -c "apt-get update && apt-get -y install make && cp /usr/bin/make /out/make"
```

### Quick Test

The following command will run the entire process (start a fabric network, create channel, test chaincode and stop it.) pass-through.

```bash
$ make setup # Install docker/compose, and pull required images
$ make test  # Test with default fabric solo mode
```

### Test with more modes

```bash
$ HLF_MODE=solo make test # Bootup a fabric network with solo mode
$ HLF_MODE=couchdb make test # Enable couchdb support, web UI is at `http://localhost:5984/_utils`
$ HLF_MODE=event make test  # Enable eventhub listener
$ HLF_MODE=kafka make test # Bootup a fabric network with kafka mode
$ HLF_MODE=be make test  # Start a blockchain-explorer to view network info
```

## Detailed Steps

See [detailed steps](docs/steps.md)

## Acknowledgement

* [Hyperledger Fabric](https://github.com/hyperledger/fabric/) project.
* [Yeasy Master](https://github.com/yeasy/docker-compose-files/tree/master/hyperledger_fabric) project.
