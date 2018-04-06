package jwt

import (
	"github.com/dgrijalva/jwt-go"
	"github.com/pkg/errors"
	"net/http"
	"github.com/unchainio/pkg/xlogger"
	"bitbucket.org/unchain/blockchaingers18/blockchain-explorer/api/pkg/config"
	"bitbucket.org/unchain/blockchaingers18/blockchain-explorer/api/pkg/domain"
	"bitbucket.org/unchain/blockchaingers18/blockchain-explorer/api/pkg/util"
	"bitbucket.org/unchain/blockchaingers18/blockchain-explorer/api/pkg/interfaces"
)

type Service struct {
	log  	*xlogger.Logger
	config  *config.ServerConfig
	util 	util.IService
}


func NewService(log *xlogger.Logger, cfg *config.ServerConfig, util util.IService) (interfaces.IJWTService, error) {
	return &Service{
		log:     log,
		config:  cfg,
		util:	 util,
	}, nil

}

func (s *Service) CreateJwtToken() (string, error) {
	jwtToken := jwt.NewWithClaims(jwt.GetSigningMethod("HS256"), &domain.User{
		Username: s.config.Username,
		Role:  s.config.Role,
	})
	token, err := jwtToken.SignedString([]byte(s.config.ApiTokenSecret))
	if err != nil {
		return "", errors.Wrap(err, "token could not be verified")
	}
	return token, nil
}

func (s *Service) CheckToken(r *http.Request) bool {
	token, err := s.util.FromRequest(r)
	if err != nil {
		s.log.Errorln(err)
		return false
	}
	user := domain.User{}
	_, err = jwt.ParseWithClaims(token, &user, func(token *jwt.Token) (interface{}, error) {
		return []byte(s.config.ApiTokenSecret), nil
	})
	if err != nil {
		s.log.Errorln(err)
		return false
	}
	return true

}