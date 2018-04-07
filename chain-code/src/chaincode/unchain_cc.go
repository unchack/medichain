package main

import (
	"fmt"

	"github.com/hyperledger/fabric/core/chaincode/shim"
	pb "github.com/hyperledger/fabric/protos/peer"
	"encoding/json"
	"bytes"
	"strconv"
)

type UnchainChaincode struct{}

func (c *UnchainChaincode) Init(stub shim.ChaincodeStubInterface) pb.Response {

	return shim.Success([]byte("Successful init"))
}

func (c *UnchainChaincode) Invoke(stub shim.ChaincodeStubInterface) pb.Response {

	function, args := stub.GetFunctionAndParameters()
	if function == "add_testdata" {
		return c.addTestdata(stub, args)
	} else if function == "register" {
		return c.register(stub, args)
	} else if function == "get_history" {
		return c.getHistoryForId(stub, args)
	}

	return shim.Error("Invalid invoke function name. Expecting \"register\" \"get_history\"")
}

func (c *UnchainChaincode) register(stub shim.ChaincodeStubInterface, args []string) pb.Response {
	// Expect two args
	//
	// args[0]: event, i.e. 0
	// args[1]: objectId, i.e. "01"
	// args[2]: timestamp, i.e. 1451293134
	if len(args) != 3 {
		msg := "Incorrect number of arguments. Expecting 2"
		return c.handleError(stub, msg)
	}

	// Fetch Object
	key := args[1]
	objBytes, err := stub.GetState(key)

	var object Object
	err = json.Unmarshal(objBytes, &object)
	if err != nil {
		return c.handleError(stub, "Could not unmarshal object")
	}

	subj, err := c.getCaller(stub)
	if err != nil {
		return c.handleError(stub, "Could not get caller")
	}
	n := bytes.IndexByte(subj, 0)

	subjID := string(subj[:n])
	timestamp, err := strconv.Atoi(args[2])
	if err != nil {
		c.handleError(stub, "Could not convert timestamp to int")
	}
	evType, err := strconv.Atoi(args[0])
	if err != nil {
		return c.handleError(stub, "Could not convert event type to int")
	}

	// Create registration event
	re := RegistrationEvent{
		SubjectID: subjID,
		TimeStamp: timestamp,
		Type: EventType(evType),
	}

	// Add registration event to object
	object.RegistrationEvents = append(object.RegistrationEvents, re)

	objBytes, err = json.Marshal(object)
	if err != nil {
		c.handleError(stub, "Could not marshal object back to bytes")
	}

	// Write object to state
	err = stub.PutState(key, objBytes)
	if err != nil {
		return c.handleError(stub, "Error writing to state.")
	}

	// Return response
	return shim.Success([]byte(`Successfully wrote registration event to object`))
}

func (c *UnchainChaincode) getHistoryForId(stub shim.ChaincodeStubInterface, args []string) pb.Response {
	// Expect one args
	//
	// args[0]: key hash
	if len(args) != 1 {
		msg := "Incorrect number of arguments. Expecting 1"
		return c.handleError(stub, msg)
	}
	key := args[0]

	// Check that hashes are of length 18
	if len(key) != 18 {
		msg := "Incorrect length of key string. Should be 18."

		return c.handleError(stub, msg)
	}

	// Write to state
	res, err := stub.GetState(key)
	if err != nil {
		return c.handleError(stub, err.Error())
	}

	// Return response
	return shim.Success([]byte(res))
}

func (c *UnchainChaincode) addTestdata(stub shim.ChaincodeStubInterface, args []string) (pb.Response) {

	// Expect one arg
	// args[0]: json object with serialized data
	if len(args) != 1 {
		msg := "Incorrect number of arguments. Expecting 2"
		return c.handleError(stub, msg)
	}
	data := args[0]

	// Deserialize data
	var objects []Object

	err := json.Unmarshal([]byte(data), &objects)
	if err != nil {
		return c.handleError(stub, "Could not unmarshal test data")
	}

	for _, obj := range objects {
		// marshal individual object back
		objBytes, err := json.Marshal(obj)
		if err != nil {
			return c.handleError(stub, "Error marshalling object")
		}
		// Write to state with object ID and json obj
		err = stub.PutState(obj.ID, objBytes)
		if err != nil {
			return c.handleError(stub, "Error writing to state.")
		}
	}
	return shim.Success([]byte("OK"))
}

func (c *UnchainChaincode) getCaller(stub shim.ChaincodeStubInterface) ([]byte, error) {
	return stub.GetCreator()
}

func (c *UnchainChaincode) handleError(stub shim.ChaincodeStubInterface, err string) pb.Response {
	fmt.Println(err)
	return shim.Error("Error.. check your chaincode container")
}

func main() {
	err := shim.Start(new(UnchainChaincode))
	if err != nil {
		fmt.Printf("Error starting Unchain chaincode1: %s", err)
	}
}

type Object struct {
	ID                 string              `json:"id"`
	OwnerID            string              `json:"ownerId"`
	Type               ObjectType          `json:"objectType"`                // [ Pallet / Case / Secondary / Primary ]
	ExpirationDate     int                 `json:"expirationDate, omitempty"` // unix epoch in ms
	Description        string              `json:"description"`
	Children           []string            `json:"children, omitempty"` // IDs of children
	RegistrationEvents []RegistrationEvent `json:"registrationEvents, omitempty"`
}

type RegistrationEvent struct {
	SubjectID string    `json:"subject"` // ID of Subject of registration - who registered?
	Type      EventType `json:"eventType"`
	TimeStamp int       `json:"timeStamp"` // unix epoch in ms
}

type ObjectType int

const (
	Pallet    ObjectType = iota
	Case
	Secondary
	Primary
)

type EventType int

const (
	Checkin  EventType = iota
	Checkout
)
