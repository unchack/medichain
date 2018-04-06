package handler

import (
	"net/http"
	"bitbucket.org/unchain/blockchaingers18/blockchain-explorer/api/pkg/domain"
)

func (h *Handler) GetUser(w http.ResponseWriter, r *http.Request) {
	if h.jwt.CheckToken(r) {
		h.util.Write(w, domain.User{
			Username:   h.config.Username,
			Role: 		h.config.Role,
		})
	} else {
		w.Write([]byte("Service Error - check token"))
	}
}
