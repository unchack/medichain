package domain

import "github.com/dgrijalva/jwt-go"

type User struct {
	Username 	string `json:"username"`
	Role 		string `json:"role"`
	jwt.StandardClaims
}

