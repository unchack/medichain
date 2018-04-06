package main

import (
	"github.com/hyperledger/fabric/core/chaincode/shim"
	"testing"
	"fmt"
	"io/ioutil"
)

func TestUnchainChaincode_Init(t *testing.T) {
	stub := createMockStub()

	res := stub.MockInit("1", [][]byte{})
	if res.Status != shim.OK {
		fmt.Println("Init failed", string(res.Message))
		t.FailNow()
	}
}

func TestUnchainChaincode_AddTestdata(t *testing.T) {
	stub := createMockStub()

	checkInit(t, stub, [][]byte{})

	serializedData, _ := ioutil.ReadFile("../fixtures/serialized-data.json")

	checkInvoke(t, stub, [][]byte{[]byte("add_testdata"), serializedData })
	checkState(t, stub, "01", "{}")
}

//func TestUnchainChaincode_QueryRecordByKey(t *testing.T) {
//	stub := createMockStub()
//
//	checkInit(t, stub, [][]byte{})
//
//	key := "asdfghjklasdfasdfa"
//	value := "012345678912345678"
//
//	checkInvoke(t, stub, [][]byte{[]byte("write_record"), []byte(key), []byte(value)})
//
//	checkQuery(t, stub, "query_record_by_key", key, value)
//}


/* Test helper functions */
func createMockStub() *shim.MockStub {
	cc := new(UnchainChaincode)
	return shim.NewMockStub("test-cc", cc)

}

func checkInit(t *testing.T, stub *shim.MockStub, args [][]byte) {
	res := stub.MockInit("1", args)
	if res.Status != shim.OK {
		fmt.Println("Init failed", string(res.Message))
		t.FailNow()
	}
}

func checkState(t *testing.T, stub *shim.MockStub, name string, value string) {
	bytes := stub.State[name]
	if bytes == nil {
		fmt.Println("State", name, "failed to get value")
		t.FailNow()
	}
	if string(bytes) != value {
		fmt.Println("State value", name, "was not", value, "as expected")
		t.FailNow()
	}
}

func checkQuery(t *testing.T, stub *shim.MockStub, fn string, name string, value string) {
	res := stub.MockInvoke("1", [][]byte{[]byte(fn), []byte(name)})
	if res.Status != shim.OK {
		fmt.Println("Query", name, "failed", string(res.Message))
		t.FailNow()
	}
	if res.Payload == nil {
		fmt.Println("Query", name, "failed to get value")
		t.FailNow()
	}
	if string(res.Payload) != value {
		fmt.Println("Query value", name, "was not", value, "as expected")
		t.FailNow()
	}
}

func checkInvoke(t *testing.T, stub *shim.MockStub, args [][]byte) {
	res := stub.MockInvoke("1", args)
	if res.Status != shim.OK {
		fmt.Println("Invoke", args, "failed", string(res.Message))
		t.FailNow()
	}
}