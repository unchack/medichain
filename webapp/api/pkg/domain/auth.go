package domain

type AuthSignInRequest struct {
	Username	 	  string `json:"username"`
	Password	 	  string `json:"password"`
}

type AuthStatusResp struct {
	Status bool `json:"status"`
}

type AuthSignOutResp struct {
	Success bool `json:"success"`
}