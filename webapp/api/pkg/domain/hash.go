package domain

import googleProtoBuf "github.com/golang/protobuf/ptypes/timestamp"

type HashValidationRespone struct {
	Key   string `json:"key"`
	Value string `json:"value"`
}

type KeyModification struct {
	TxId      string                    `json:"tx_id,omitempty"`
	Value     []byte                    `json:"value,omitempty"`
	Timestamp *googleProtoBuf.Timestamp `json:"timestamp,omitempty"`
	IsDelete  bool                      `json:"is_delete,omitempty"`
}
