package interfaces

import (
	"net/http"
	"bitbucket.org/unchain/blockchaingers18/blockchain-explorer/api/pkg/domain"
	"github.com/pressly/chi"
)

type IHandler interface {
	GetUser(w http.ResponseWriter, r *http.Request)
	Login(w http.ResponseWriter, r *http.Request)
	Logout(w http.ResponseWriter, r *http.Request)
	CheckAuthenticated(w http.ResponseWriter, r *http.Request)
}

type IAPIService interface {
	DefineRoutes() *chi.Mux
	RunHttpListener()
}

type IHashService interface {
	ValidateHashOnBlockchain(hashArray string) (domain.HashValidationRespone, error)
}

type IJWTService interface {
	CreateJwtToken() (string, error)
	CheckToken(r *http.Request) bool
}

type IUtilService interface {
	FromRequest(req *http.Request) (string, error)
	Write(w http.ResponseWriter, i interface{})
}