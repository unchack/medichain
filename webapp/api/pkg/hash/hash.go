package hash

import (
	"net/http"
	"github.com/unchainio/pkg/xlogger"
	"bitbucket.org/unchain/blockchaingers18/blockchain-explorer/api/pkg/config"
	"bitbucket.org/unchain/blockchaingers18/blockchain-explorer/api/pkg/interfaces"
)

type Service struct {
	log    *xlogger.Logger
	config *config.ServerConfig
	client *http.Client
}

func NewService(log *xlogger.Logger, cfg *config.ServerConfig, util interfaces.IUtilService) (interfaces.IHashService, error) {
	var err error
	return &Service{
		log:    log,
		config: cfg,
		client: &http.Client{},
	}, err

}
