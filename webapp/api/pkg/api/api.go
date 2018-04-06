package api

import (
	"github.com/unchainio/pkg/xlogger"
	"bitbucket.org/unchain/blockchaingers18/blockchain-explorer/api/pkg/config"
	"github.com/pressly/chi"
	"net/http"
	"strings"
	"bitbucket.org/unchain/blockchaingers18/blockchain-explorer/api/pkg/interfaces"
)

type Service struct {
	log  	*xlogger.Logger
	config  *config.ServerConfig
	handler interfaces.IHandler
}


func NewService(log *xlogger.Logger, cfg *config.ServerConfig, h interfaces.IHandler) (interfaces.IAPIService, error) {
	return &Service{
		log:     log,
		config:  cfg,
		handler: h,
	}, nil
}

func (s *Service) RunHttpListener() {
	s.log.Println("starting http listener on", s.config.ApiPort)
	r := s.DefineRoutes()
	s.fileServer(r, "/", http.Dir(s.config.ClientPath))
	s.log.Fatal(http.ListenAndServe(s.config.ApiPort, r))
}

// fileServer conveniently sets up a http.FileServer handler to serve
// static files from a http.FileSystem.
func (s *Service) fileServer(r chi.Router, path string, root http.FileSystem) {
	if strings.ContainsAny(path, "{}*") {
		panic("FileServer does not permit URL parameters.")
	}

	fs := http.StripPrefix(path, http.FileServer(root))

	if path != "/" && path[len(path)-1] != '/' {
		r.Get(path, http.RedirectHandler(path+"/", 301).ServeHTTP)
		path += "/"
	}
	path += "*"

	r.Get(path, http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		fs.ServeHTTP(w, r)
	}))
}
