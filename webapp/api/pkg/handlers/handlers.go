package handler

import (
	"bitbucket.org/unchain/blockchaingers18/blockchain-explorer/api/pkg/config"
	"github.com/unchainio/pkg/xlogger"
	"bitbucket.org/unchain/blockchaingers18/blockchain-explorer/api/pkg/interfaces"
)

type Handler struct {
	config		*config.ServerConfig
	log 		*xlogger.Logger
	util 		interfaces.IUtilService
	jwt 		interfaces.IJWTService
	hash		interfaces.IHashService
}

func NewHandler(log *xlogger.Logger,
				cfg *config.ServerConfig,
				j interfaces.IJWTService,
				u interfaces.IUtilService,
				h interfaces.IHashService) (interfaces.IHandler, error) {
	return &Handler{
		log:     log,
		config:  cfg,
		util:	 u,
		jwt:	 j,
		hash: 	 h,
	}, nil
}