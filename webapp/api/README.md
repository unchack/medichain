<p align="center">
  <a href="https://www.unchain.io/" target='_blank'>
    <img alt="Unchain" title="Unchain POC Logo" src="https://pbs.twimg.com/profile_images/976939951362330624/D8ls1d9A_400x400.jpg" width="100" height="100">
  </a>
</p>

<p align="center">
  Unchain proof of concept api backend application built with golang,
    <a href="https://github.com/go-chi/chi/" target='_blank'>chi-router</a>,
    <a href="https://github.com/olivere/elastic/" target='_blank'>elastic-client</a>
</p>

## Cody style/organization and philosophy

[Standard Package Layout](https://medium.com/@benbjohnson/standard-package-layout-7cdbc8391fc1)

[Go best practices, six years in](https://peter.bourgon.org/go-best-practices-2016/)

## Prequisites

Install golang! - Why haven't you already done so?

Setup appropriate goPath/goRoot/goBin.

Clone this repository into your appropriate go path

eg:

`~/go/src/bitbucket.org/unchain/blockchaingers18/blockchain-explorer/api`

Install dep: go get -u github.com/golang/dep/...

Run dep ensure

## Development server

`
    go run cmd/webapp/main.go
`


## Configurable Properties

The app configuration file can be found in api/fixtures/api.toml


    apiPort: port the backend will be exposed on
    validationAdapterUrl: host:port where the blockchain gateway adapter will be listening
    clientPath: path from which the production ready client build will be served from
    username: simple PoC magic
    password: simple PoC magic
    role:     simple PoC magic
    apiTokenSecret: jwt server token used to create jwt authentication tokens for the client
    jwtSchema: use Basic or Bearer depending on you jwt schema leave blank if you dont use any


## Hash operations explained

The following structure is saved in the blockchain.

key: value or hashOfKey: value

The order defined in the array is the order used to create the hash.

The key hash is calculated by the backend in order to query those key in the blockchain.


## Who are we?

We are [Unchain BV](https://unchain.io).