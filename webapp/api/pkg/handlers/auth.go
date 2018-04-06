package handler

import (
	"encoding/json"
	"bytes"
	"net/http"
	"bitbucket.org/unchain/blockchaingers18/blockchain-explorer/api/pkg/domain"
)

func (h *Handler) Login(w http.ResponseWriter, r *http.Request) {
	defer r.Body.Close()
	decoder := json.NewDecoder(r.Body)
	var request domain.AuthSignInRequest
	err := decoder.Decode(&request)
	if err != nil {
		w.Write([]byte("Service Error - decode body"))
	} else {
		if h.config.Username == request.Username &&
			h.config.Password == request.Password {
			user := domain.User{
				Username:   h.config.Username,
				Role: 		h.config.Role,
			}
			b := new(bytes.Buffer)
			json.NewEncoder(b).Encode(user)
			token , err := h.jwt.CreateJwtToken()
			if err != nil {
				w.Write([]byte("Service Error - create token"))
			}
			schemaAndToken := h.config.JwtSchema + token

			w.Header().Set("Access-Token", schemaAndToken)
			w.Header().Set("Content-Type", "application/json")
			w.Header().Set("Access-Control-Expose-Headers", "Access-Token")

			w.Write(b.Bytes())
		}
	}
}

func (h *Handler) Logout(w http.ResponseWriter, r *http.Request) {
	if h.jwt.CheckToken(r) {
		h.util.Write(w, domain.AuthSignOutResp{
			Success: true,
		})
	} else {
		w.Write([]byte("Service Error - check token"))
	}
}

func (h *Handler) CheckAuthenticated(w http.ResponseWriter, r *http.Request) {
	h.util.Write(w, domain.AuthStatusResp{
		Status: h.jwt.CheckToken(r),
	})
}
