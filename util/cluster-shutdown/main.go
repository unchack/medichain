package main

import (
	"github.com/unchainio/pkg/xexec"
	"fmt"
)

func main()  {
	// ADD YOUR PI HOSTNAMES HERE
	hosts := []string{"pi-1", "pi-2", "pi-3", "pi-4", "pi-commander-1"}

	username := "pi"

	cmd := "sudo shutdown now"

	for _, hostName := range hosts {
		host := fmt.Sprintf("%s@%s.local", username, hostName)
		if _, err := xexec.Run(`
ssh %s <<'ENDSSH'
	%s
ENDSSH`, xexec.WithFormat(host, cmd), xexec.StreamOutput()); err != nil {
			fmt.Println(err)
		}
	}
}
