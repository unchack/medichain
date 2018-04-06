package api

import (
	"github.com/pressly/chi"
)

func (s *Service) DefineRoutes() (*chi.Mux) {
	r := chi.NewRouter()

	r.Options("/auth/sign_in", s.handler.Login)
	r.Post("/auth/sign_in", s.handler.Login)

	r.Options("/auth/sign_out", s.handler.Logout)
	r.Delete("/auth/sign_out", s.handler.Logout)

	r.Options("/users/check_authenticated", s.handler.CheckAuthenticated)
	r.Get("/users/check_authenticated", s.handler.CheckAuthenticated)

	r.Options("/users/whoami", s.handler.GetUser)
	r.Get("/users/whoami", s.handler.GetUser)

	// todo implement client query handler on event see client/src/app/shared/services/api.service.ts

	return r
}

