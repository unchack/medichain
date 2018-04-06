package xhfc

import (
	"github.com/unchainio/interfaces/logger"

	"github.com/hyperledger/fabric-sdk-go/api/apiconfig"
	fab "github.com/hyperledger/fabric-sdk-go/api/apifabclient"
	chmgmt "github.com/hyperledger/fabric-sdk-go/api/apitxn/chmgmtclient"
	resmgmt "github.com/hyperledger/fabric-sdk-go/api/apitxn/resmgmtclient"
	"github.com/hyperledger/fabric-sdk-go/pkg/config"
	"github.com/hyperledger/fabric-sdk-go/pkg/fabric-client/chconfig"
	"github.com/hyperledger/fabric-sdk-go/pkg/fabsdk"
	pb "github.com/hyperledger/fabric-sdk-go/third_party/github.com/hyperledger/fabric/protos/peer"
	"github.com/pkg/errors"
)

// BaseSetupImpl implementation of BaseTestSetup
type BaseSetupImpl struct {
	SDK             *fabsdk.FabricSDK
	Identity        fab.IdentityContext
	Client          fab.Resource
	Channel         fab.Channel
	EventHub        fab.EventHub
	ConnectEventHub bool
	OrgID           string
	ChannelID       string
	ChainCodeID     string
	Initialized     bool
	ChannelConfig   string

	ConfigByte []byte
	ConfigType string
	Log        logger.Logger
}

var resMgmtClient resmgmt.ResourceMgmtClient

func (setup *BaseSetupImpl) Initialize() error {
	// Create SDK setup for the integration tests
	sdk, err := fabsdk.New(config.FromRaw(setup.ConfigByte, setup.ConfigType))

	if err != nil {
		return errors.WithMessage(err, "SDK init failed")
	}
	setup.SDK = sdk

	session, err := sdk.NewClient(fabsdk.WithUser("Admin"), fabsdk.WithOrg(setup.OrgID)).Session()
	if err != nil {
		return errors.WithMessage(err, "failed getting admin user session for org")
	}

	setup.Identity = session

	sc, err := sdk.FabricProvider().CreateResourceClient(setup.Identity)
	if err != nil {
		return errors.WithMessage(err, "NewResourceClient failed")
	}

	setup.Client = sc

	// TODO: Review logic for retrieving peers (should this be channel peer only)
	channel, err := GetChannel(sdk, setup.Identity, sdk.Config(), chconfig.NewChannelCfg(setup.ChannelID), []string{setup.OrgID})
	if err != nil {
		return errors.Wrapf(err, "create channel (%s) failed: %v", setup.ChannelID)
	}

	targets := []fab.ProposalProcessor{channel.PrimaryPeer()}
	req := chmgmt.SaveChannelRequest{ChannelID: setup.ChannelID, ChannelConfig: setup.ChannelConfig, SigningIdentity: session}
	InitializeChannel(sdk, setup.OrgID, req, targets)

	if err := setup.setupEventHub(sdk, setup.Identity); err != nil {
		return err
	}

	// At this point we are able to retrieve channel configuration
	configProvider, err := sdk.FabricProvider().CreateChannelConfig(setup.Identity, setup.ChannelID)
	if err != nil {
		return err
	}
	chCfg, err := configProvider.Query()
	if err != nil {
		return err
	}

	// Get channel from dynamic info
	channel, err = GetChannel(sdk, setup.Identity, sdk.Config(), chCfg, []string{setup.OrgID})
	if err != nil {
		return errors.Wrapf(err, "create channel (%s) failed: %v", setup.ChannelID)
	}
	setup.Channel = channel

	setup.Initialized = true

	return nil
}

// GetChannel initializes and returns a channel based on config
func GetChannel(sdk *fabsdk.FabricSDK, ic fab.IdentityContext, config apiconfig.Config, chCfg fab.ChannelCfg, orgs []string) (fab.Channel, error) {

	channel, err := sdk.FabricProvider().CreateChannelClient(ic, chCfg)
	if err != nil {
		return nil, errors.WithMessage(err, "NewChannel failed")
	}

	for _, org := range orgs {
		peerConfig, err := config.PeersConfig(org)
		if err != nil {
			return nil, errors.WithMessage(err, "reading peer config failed")
		}
		for _, p := range peerConfig {
			endorser, err := sdk.FabricProvider().CreatePeerFromConfig(&apiconfig.NetworkPeer{PeerConfig: p})
			if err != nil {
				return nil, errors.WithMessage(err, "NewPeer failed")
			}
			err = channel.AddPeer(endorser)
			if err != nil {
				return nil, errors.WithMessage(err, "adding peer failed")
			}
		}
	}

	return channel, nil
}

func (setup *BaseSetupImpl) setupEventHub(client *fabsdk.FabricSDK, identity fab.IdentityContext) error {
	eventHub, err := client.FabricProvider().CreateEventHub(identity, setup.ChannelID)
	if err != nil {
		return err
	}

	if setup.ConnectEventHub {
		if err := eventHub.Connect(); err != nil {
			return errors.WithMessage(err, "eventHub connect failed")
		}
	}
	setup.EventHub = eventHub

	return nil
}

func (setup *BaseSetupImpl) CreateAndSendTransactionProposal(channel fab.Channel, chainCodeID string,
	fcn string, args [][]byte, targets []fab.ProposalProcessor, transientMap map[string][]byte) ([]*fab.TransactionProposalResponse, fab.TransactionID, error) {

	request := fab.ChaincodeInvokeRequest{
		Fcn:          fcn,
		Args:         args,
		TransientMap: transientMap,
		ChaincodeID:  chainCodeID,
	}

	transactionProposalResponses, txnID, err := channel.SendTransactionProposal(request, targets)

	if err != nil {
		return nil, txnID, err
	}

	return transactionProposalResponses, txnID, nil
}

// CreateAndSendTransaction ...
func (setup *BaseSetupImpl) CreateAndSendTransaction(channel fab.Channel, resps []*fab.TransactionProposalResponse) (*fab.TransactionResponse, error) {

	tx, err := channel.CreateTransaction(resps)
	if err != nil {
		return nil, errors.WithMessage(err, "CreateTransaction failed")
	}

	transactionResponse, err := channel.SendTransaction(tx)
	if err != nil {
		return nil, errors.WithMessage(err, "SendTransaction failed")

	}

	if transactionResponse.Err != nil {
		return nil, errors.Wrapf(transactionResponse.Err, "orderer %s failed", transactionResponse.Orderer)
	}

	return transactionResponse, nil
}

// RegisterTxEvent registers on the given eventhub for the give transaction
// returns a boolean channel which receives true when the event is complete
// and an error channel for errors
// TODO - Duplicate
func (setup *BaseSetupImpl) RegisterTxEvent(txID fab.TransactionID, eventHub fab.EventHub) (chan bool, chan error) {
	done := make(chan bool)
	fail := make(chan error)

	eventHub.RegisterTxEvent(txID, func(txId string, errorCode pb.TxValidationCode, err error) {
		if err != nil {
			setup.Log.Errorf("Received error event for txid(%s)", txId)
			fail <- err
		} else {
			setup.Log.Printf("Received success event for txid(%s)", txId)
			done <- true
		}
	})

	return done, fail
}

// invokeChaincode ...
func (setup *BaseSetupImpl) InvokeChaincode(fcn string, args [][]byte, transientMap map[string][]byte) ([]*fab.TransactionProposalResponse, *fab.TransactionID, error) {

	responses, txID, err := setup.CreateAndSendTransactionProposal(
		setup.Channel,
		setup.ChainCodeID,
		fcn,
		args,
		[]fab.ProposalProcessor{setup.Channel.PrimaryPeer()},
		transientMap,
	)

	if err != nil {
		return nil, nil, err
	}

	_, err = setup.CreateAndSendTransaction(setup.Channel, responses)

	if err != nil {
		return nil, nil, err
	}

	return responses, &txID, nil
}
