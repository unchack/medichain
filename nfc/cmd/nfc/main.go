package nfc

import (
	"fmt"
	"os"
	"syscall"
	"time"
	"os/signal"
	"bitbucket.org/unchain-bc1718/medchain/nfc/pkg/nxprd"
	"github.com/pkg/errors"
	"bitbucket.org/unchain-bc1718/medchain/nfc/pkg/xhfc"
	"github.com/hyperledger/fabric-sdk-go/api/apifabclient"
	"github.com/unchainio/pkg/xconfig"
	"github.com/unchainio/pkg/xerrors"
	"github.com/unchainio/pkg/xlogger"
)

func cleanup() {
	// In order to cleanup the C part of the wrapper DeInit need to be called.
	nxprd.DeInit()
	fmt.Println("Clean up reader interface - done")
}

type Response struct {
	responses []*apifabclient.TransactionProposalResponse
	txID      *apifabclient.TransactionID
}

type Config struct {
	Logger         		  *xlogger.Config 	  `yaml:"logger"`
	AttributeMap          map[string][]string `yaml:"attributesMap"`
	ChannelName           string              `yaml:"channelName"`
	HFCUsername           string              `yaml:"hfcUsername"`
	HFCOrgID              string              `yaml:"hfcOrgID"`
	NetworkConfigFilePath string              `yaml:"networkConfigFilePath"`
	ChannelConfigFilePath string              `yaml:"channelConfigFilePath"`
	KeyValueStorePath     string              `yaml:"keyValueStorePath"`
	ChainCodeID           string              `yaml:"chainCodeID"`
	FabricConfigPath	  string			  `yaml:"fabricConfigPath"`
}


func main() {
	c := make(chan os.Signal, 2)
	signal.Notify(c, os.Interrupt, syscall.SIGTERM)
	go func() {
		<-c
		cleanup()
		os.Exit(1)
	}()

	cfg := new(Config)

	log, err := xlogger.New(cfg.Logger)
	xerrors.PanicIfError(err)

	err = xconfig.Load(
		cfg,
		xconfig.FromPathFlag("cfg", "config/dev/hfc-config.yaml"),
		xconfig.FromEnv(),
		xconfig.Verbose(true),
	)
	xerrors.PanicIfError(err)

	if err != nil {
		xerrors.PanicIfError(errors.New(fmt.Sprintf("Could not unmarshal config: %s", err)))
	}

	hfcClient := xhfc.BaseSetupImpl{
		ConfigPath:      cfg.FabricConfigPath,
		ChannelID:       cfg.ChannelName,
		OrgID:           cfg.HFCOrgID,
		ChannelConfig:   cfg.ChannelConfigFilePath,
		ConnectEventHub: true,
		ChainCodeID:     cfg.ChainCodeID,
		Log:             log,
	}

	err = hfcClient.Initialize()

	if err != nil {
		xerrors.PanicIfError(errors.WithMessage(err, "failed to initialize hyperledger fabric SDK"))
	}


	for {
		// Initialize the library. We need to do that once.
		if err := nxprd.Init(); err != nil {
			xerrors.PanicIfError(errors.WithMessage(err, "failed to initialize NXP library"))
		}

		// Try to detect/discover a card/tag for 1000ms. Discover will block.
		// 1000ms is the default timeout.
		dev, err := nxprd.Discover(10000)
		if err != nil {
			log.Println("\nCouldn't detect card")
		} else {
			if dev.Params.DevType == "Unknown" {
				// A card/tag could be detected, but the wrapper doesn't support it yet.
				// So we can't read or write blocks, but we can access some parameters.
				log.Println("\nFound an unknown card")
				log.Println("")
			} else {
				log.Println("\nFound a known card")
				time.Sleep(2 * time.Second)
			}
		}
		// In order to cleanup the C part of the wrapper DeInit need to be called.
		nxprd.DeInit()
		fmt.Println("Clean up reader interface - done")
	}
}