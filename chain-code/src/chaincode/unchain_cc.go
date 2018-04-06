package main

import (
	"fmt"

	"github.com/hyperledger/fabric/core/chaincode/shim"
	pb "github.com/hyperledger/fabric/protos/peer"
)

type UnchainChaincode struct{}

func (c *UnchainChaincode) Init(stub shim.ChaincodeStubInterface) pb.Response {

	return shim.Success([]byte("Successful init"))
}

func (c *UnchainChaincode) Invoke(stub shim.ChaincodeStubInterface) pb.Response {

	function, args := stub.GetFunctionAndParameters()
	if function == "write_record" {
		return c.writeRecord(stub, args)
	} else if function == "query_record_by_key" {
		return c.queryRecordByKey(stub, args)
	}

	return shim.Error("Invalid invoke function name. Expecting \"write_record\" \"query_record_by_key\"")
}

func (c *UnchainChaincode) writeRecord(stub shim.ChaincodeStubInterface, args []string) pb.Response {
	// Expect two args
	//
	// args[0]: key hash
	// args[1]: value hash
	if len(args) != 2 {
		msg := "Incorrect number of arguments. Expecting 2"
		return c.handleError(stub, msg)
	}
	key := args[0]
	value := args[1]

	// Check that hashes are of length 18
	if len(key) != 18 && len(value) != 18 {
		msg := "Incorrect length of key or value string. Should be 18."
		return c.handleError(stub, msg)
	}

	// Write to state
	err := stub.PutState(key, []byte(value))
	if err != nil {
		return c.handleError(stub, "Error writing to state.")
	}

	// Return response
	return shim.Success([]byte(`Successfully wrote key-value to ledger state`))
}

func (c *UnchainChaincode) queryRecordByKey(stub shim.ChaincodeStubInterface, args []string) pb.Response {
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