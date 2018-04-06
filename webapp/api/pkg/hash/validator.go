package hash

import (
	"bytes"
	"encoding/json"
	"net/http"

	"github.com/pkg/errors"
	"bitbucket.org/unchain/blockchaingers18/blockchain-explorer/api/pkg/domain"
)

func (s *Service) ValidateHashOnBlockchain(hash string) (domain.HashValidationRespone, error) {
	var hashPair domain.HashValidationRespone
	body, err := json.Marshal(hash)
	if err != nil {
		return hashPair, errors.Wrap(err, "could not marshal hash array")
	}

	req, err := http.NewRequest("POST", s.config.AdapterUrl, bytes.NewBuffer(body))
	if err != nil {
		return hashPair, errors.Wrap(err, "could not set up validation request")
	}
	req.Header.Set("Content-Type", "application/json")

	resp, err := s.client.Do(req)
	if err != nil {
		return hashPair, errors.Wrap(err, "could not do validation request")
	}
	defer resp.Body.Close()
	decoder := json.NewDecoder(resp.Body)

	err = decoder.Decode(&hashPair)
	if err != nil {
		return hashPair, errors.Wrap(err, "could not decode hash pairs")
	}
	return hashPair, nil
}
