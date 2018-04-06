<p align="center">
  <a href="https://www.unchain.io/" target='_blank'>
    <img alt="Unchain" title="Unchain POC Logo" src="https://pbs.twimg.com/profile_images/976939951362330624/D8ls1d9A_400x400.jpg" width="100" height="100">
  </a>
</p>

<p align="center">
  UnchainIO proof of concept client application built with Angular5, Redux/ngrx-store4, Observables & ImmutableJs.
</p>

## Development server

Since the development server of the front end and the api backend run on different ports.

Make sure to enable a cors browser extension setting a wildcard url pattern expression like so:
*://localhost/*

Run `yarn start` for a dev server. Navigate to `http://localhost:4200/`. The app will automatically reload if you change any of the source files.

## Code scaffolding

Run `ng generate component component-name` to generate a new component. You can also use `ng generate directive|pipe|service|class|guard|interface|enum|module`.

## Build

Run `yarn build` to build the project. The build artifacts will be stored in the `last-build/` directory. Use the `-prod` flag for a production build.

## Prod Build
Run `yarn build:prod` to build the project. The build artifacts will be stored in the `last-build/` directory.

## Running unit tests

Run `ng test` to execute the unit tests via [Karma](https://karma-runner.github.io).

## Running end-to-end tests

Run `ng e2e` to execute the end-to-end tests via [Protractor](http://www.protractortest.org/).

## Serving the production build from the backend api

If last-build does not exist run cd ./client && yarn run build:prod

Run docker-compose up from the root of the project

## Configurable Properties

The app configuration file can be found in client/src/environments

  API_ENDPOINT: 'http://<api-backend-host>:8787/'

## Who are we?

We are [Unchain BV](https://unchain.io).
