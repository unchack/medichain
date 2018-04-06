# Developing actions and endpoints
This is a guideline for developing actions and endpoints for developers. It describes what type of components there are and what interfaces they use.

## What is an endpoint or action component?
Janus is an integration engine that does protocol- and data conversion. It can be configured to use plugins in order to connect to different systems using a number of protocols and run data conversions on these message.

### Endpoints
We call the protocol components endpoints. Endpoints are focused around a specific protocol and can be customized to integrate with a specific application.
An example is an AMQP endpoint that consumes messages from a queue. It can also be more specifically aimed at one application, for instance an http based endpoint that is set up to receive messages from one specific application.
The interface of an endpoint can be found [here](https://github.com/unchainio/interfaces/blob/master/plugins/plugins.go#L32).

### Actions
An action is a component that does data conversion, in essence it's a component that can run conversions or business logic on a message object. An example is a conversion from XML to JSON, or from a specific semantic message format to another, i.e. UBL to EDIFACT.
The interface of an action can be found [here](https://github.com/unchainio/interfaces/blob/master/plugins/plugins.go#L41).

### Pipelines, Message and attributes
The janus integration engine brings endpoints and actions together through pipelines. The main configuration file of the integration engine takes a list of endpoints, actions and pipelines. These pipelines basically tell the integration engine what to do with a message that comes in at a certain endpoint. It is possible to chain actions together and then indicate what the outgoing endpoint for the pipeline should be.
Throughout the pipeline the integration engine works with a specific format for a Message. The Message object can be found [here](https://github.com/unchainio/interfaces/blob/master/plugins/plugins.go#L5).
Attributes can be added and removed on a Message object. This will help distinguish messages throughout the pipeline for routing purposes (future functionality) and can be used by the components to interpret messages easier. For instance, it can be used to send a message to a specific smart contract function without having to parse the message.

## What is the end deliverable of a component?
A finished component has the following three files:
1. Plugin file: _youraction.action.so_ or _yourendpoint.endpoint.so_
    This is the actual plugin - the fruits of your labour!
2. README.md: a file describing the functionality of the component and giving an overview of the inputs and outputs.
    What do people need to know when they want to use your plugin? This is where they should be able to find it. Describe in the file the attributes it sets, the type of message it expects, its functionality and the intricacies of your endpoint.
3. config.json: a JSON template/schema for all the configuration your component requires.
    Your component will very likely require some configuration. This config.json is a template file that serves as a schema for the configuration that your plugin receives via the Initialize(config []bytes) function. The configuration of your component will need to be embedded in the main config.json of your janus adapter instance.

## Getting started
1.  Add the interfaces package to your GOPATH with: `go get github.com/unchainio/interfaces/...`
2.  Copy paste a template from the templates folder and rename it to your action or endpoint: `mv templates/action plugins/yourcomponentname.action`
    Please follow the naming convention of youraction.action or yourendpoint.endpoint.
3.  Go for it!

## Dependency management of plugins
Shared dependencies with janus-go are troublesome and will result in the following error `panic: could not open plugin: plugin.Open: plugin was built with a different version of package plugin`

## FAQ / troubleshooting