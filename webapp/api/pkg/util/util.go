package util

import (
	"strings"
	"bytes"
	"encoding/json"
	"github.com/unchainio/pkg/xlogger"
	"net/http"
	"github.com/pkg/errors"
	"bitbucket.org/unchain/blockchaingers18/blockchain-explorer/api/pkg/config"
	"bitbucket.org/unchain/blockchaingers18/blockchain-explorer/api/pkg/interfaces"
)

type Service struct {
	log  	*xlogger.Logger
	config  *config.ServerConfig
}

func NewService(log *xlogger.Logger, cfg *config.ServerConfig) (interfaces.IUtilService, error) {
	return &Service{
		log:     log,
		config:  cfg,
	}, nil
}

// FromRequest extracts the auth token from req.
func (s *Service) FromRequest(req *http.Request) (string, error) {
	// Grab the raw Authoirzation header
	authHeader := req.Header.Get("Access-Token")
	if authHeader == "" {
		return "", errors.New("authorization header required")
	}
	// Confirm the request is sending Basic Authentication credentials.
	if !strings.HasPrefix(authHeader, s.config.JwtSchema) {
		return "", errors.New("authorization requires Basic/Bearer scheme")
	}

	// Get the token from the request header
	// The first six characters are skipped - e.g. "Bearer ".
	return authHeader[len(s.config.JwtSchema):], nil
}

func (s *Service) Write(w http.ResponseWriter, i interface{}) {
	b := new(bytes.Buffer)
	json.NewEncoder(b).Encode(i)
	w.Write(b.Bytes())
}