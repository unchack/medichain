/*
Copyright SecureKey Technologies Inc. All Rights Reserved.

SPDX-License-Identifier: Apache-2.0
*/
package main

import (
	"github.com/hyperledger/fabric-sdk-go/third_party/github.com/hyperledger/fabric/common/cauthdsl"
	resmgmt "github.com/hyperledger/fabric-sdk-go/api/apitxn/resmgmtclient"

	"github.com/hyperledger/fabric-sdk-go/pkg/config"
	packager "github.com/hyperledger/fabric-sdk-go/pkg/fabric-client/ccpackager/gopackager"
	"github.com/hyperledger/fabric-sdk-go/pkg/fabsdk"
	"fmt"
)

const (
	channelID = "businesschannel"
	orgName = "Org1"
	orgAdmin = "Admin"
	ccID = "unchain_cc"
	ccPath = "chaincode/"
	ccVersion = "v3"
)

// Run enables testing an end-to-end scenario against the supplied SDK options
func main() {

	config := config.FromFile("config/hfcv1.endpoint.yaml")

	sdk, err := fabsdk.New(config)
	if err != nil {
		panic("Could not init SDK")
	}

	// Channel management client is responsible for managing channels (create/update channel)
	// Supply user that has privileges to create channel (in this case orderer admin)
	//chMgmtClient, err := sdk.NewClient(fabsdk.WithUser("Admin"), fabsdk.WithOrg("ordererorg")).ChannelMgmt()
	//if err != nil {
	//	panic("Failed to create channel management client: %s")
	//}
	//
	//// Org admin user is signing user for creating channel
	//session, err := sdk.NewClient(fabsdk.WithUser(orgAdmin), fabsdk.WithOrg(orgName)).Session()
	//if err != nil {
	//	panic("Failed to get session for %s, %s: %s")
	//}
	//orgAdminUser := session

	// Create channel
	//req := chmgmt.SaveChannelRequest{ChannelID: channelID, ChannelConfig: path.Join("../janus-nfc/config/plugins/", "mychannel.tx"), SigningIdentity: orgAdminUser}
	//if err = chMgmtClient.SaveChannel(req); err != nil {
	//	panic("cannot save channel")
	//}
	//
	//// Allow orderer to process channel creation
	//time.Sleep(time.Second * 5)

	// Org resource management client
	orgResMgmt, err := sdk.NewClient(fabsdk.WithUser(orgAdmin)).ResourceMgmt()
	if err != nil {
		panic("Failed to create new resource management client: %s")
	}

	// Org peers join channel
	//if err = orgResMgmt.JoinChannel(channelID); err != nil {
	//	panic(err)
	//}

	// Create chaincode package for unchain cc
	ccPkg, err := packager.NewCCPackage(ccPath, ".")
	if err != nil {
		panic(err)
	}

	// Install example cc to org peers
	installCCReq := resmgmt.InstallCCRequest{Name: ccID, Path: ccPath, Version: ccVersion, Package: ccPkg}
	_, err = orgResMgmt.InstallCC(installCCReq)
	if err != nil {
		panic(err)
	}

	// Set up chaincode policy
	ccPolicy := cauthdsl.SignedByAnyMember([]string{"Org1MSP", "Org2MSP", "Org3MSP"})

	// Org resource manager will instantiate 'unchain_cc' on channel
	err = orgResMgmt.InstantiateCC(channelID, resmgmt.InstantiateCCRequest{Name: ccID, Path: ccPath, Version: ccVersion, Args: nil, Policy: ccPolicy})
	if err != nil {
		panic(err)
	}

	fmt.Println("---------- Deployed Chaincode -----------")
}
