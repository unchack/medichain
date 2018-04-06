package config

import (
	"io/ioutil"

	"github.com/pkg/errors"
	"github.com/BurntSushi/toml"
)

func LoadConfig(cfg interface{}, path string) error {
	bytes, err := ioutil.ReadFile(path)

	if err != nil {
		return errors.Wrap(err, "unable to read file")
	}

	err = toml.Unmarshal(bytes, cfg)

	if err != nil {
		return errors.Wrapf(err, "error while parsing config file %s", string(bytes))
	}

	return nil
}

type ServerConfig struct {
	ApiPort                     string 			`toml:"apiPort"`
	AdapterUrl					string			`toml:"adapterUrl"`
	ClientPath                  string 			`toml:"clientPath"`
	Username					string 			`toml:"username"`
	Password					string 			`toml:"password"`
	Role						string 			`toml:"role"`
	ApiTokenSecret				string 			`toml:"apiTokenSecret"`
	JwtSchema					string 			`toml:"jwtSchema"`
}