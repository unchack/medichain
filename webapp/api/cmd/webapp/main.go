// Plugin elastic_endpoint provides an endpoint for elastic v6
package webapp

import (
	"github.com/namsral/flag"
	"github.com/unchainio/pkg/xlogger"
	"github.com/unchainio/pkg/xerrors"
	"bitbucket.org/unchain/blockchaingers18/blockchain-explorer/api/pkg/api"
	"bitbucket.org/unchain/blockchaingers18/blockchain-explorer/api/pkg/config"
	"bitbucket.org/unchain/blockchaingers18/blockchain-explorer/api/pkg/handlers"
	"bitbucket.org/unchain/blockchaingers18/blockchain-explorer/api/pkg/util"
	"bitbucket.org/unchain/blockchaingers18/blockchain-explorer/api/pkg/jwt"
	"bitbucket.org/unchain/blockchaingers18/blockchain-explorer/api/pkg/hash"
)

var defaultConfig = &xlogger.Config{
	Level:  "debug",
	Format: "text",
}

func main() {
	cfgPath := ""
	flag.StringVar(&cfgPath, "cfg", "fixtures/api.toml", "Path to the config file, defaults to fixtures/api.toml")
	flag.Parse()
	cfg := new(config.ServerConfig)
	err := config.LoadConfig(cfg, cfgPath)

	log, err := xlogger.New(defaultConfig)

	log.Printf("Loaded config from `%s`\n", cfgPath)

	if err != nil {
		panic("Could not init log")
	}

	utilService, err := util.NewService(log, cfg)
	xerrors.PanicIfError(err)
	jwtService, err := jwt.NewService(log, cfg, utilService)
	xerrors.PanicIfError(err)
	hashService, err := hash.NewService(log, cfg, utilService)
	xerrors.PanicIfError(err)
	routes, err := handler.NewHandler(log, cfg, jwtService, utilService, hashService)
	xerrors.PanicIfError(err)
	api, err := api.NewService(log, cfg, routes)
	xerrors.PanicIfError(err)
	api.RunHttpListener()
}
