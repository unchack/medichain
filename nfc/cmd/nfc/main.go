package nfc

import (
	"bytes"
	"fmt"
	"strings"

	"os"
	"syscall"
	"time"
	"os/signal"
	"bitbucket.org/unchain-bc1718/medchain/nfc/pkg/nxprd"
)

func getTag(t nxprd.TagType) string {
	switch t {
	case nxprd.TagType2:
		return "2"
	default:
		return "Undefined"
	}
}

func getTech(t nxprd.TechType) string {
	switch t {
	case nxprd.TechA:
		return "A"
	default:
		return "Undefined"
	}
}

func slice2Str(arr []byte) string {
	var buffer bytes.Buffer

	for i := 0; i < len(arr); i++ {
		buffer.WriteString(fmt.Sprintf("0x%02X ", arr[i]))
	}

	return strings.TrimSpace(buffer.String())
}

func printInfo(dev *nxprd.Device) {
	fmt.Printf("Card            : %s\n", dev.Params.DevType)
	fmt.Printf("Tag type        : %s\n", getTag(dev.Params.TagType))
	fmt.Printf("Technology type : %s\n", getTech(dev.Params.TechType))
	fmt.Printf("UID             : %s\n", slice2Str(dev.Params.UID))
	fmt.Printf("ATQ(A)          : %s\n", slice2Str(dev.Params.ATQ))
	fmt.Printf("SAK             : 0x%02X\n", dev.Params.SAK)
}

func cleanup() {
	// In order to cleanup the C part of the wrapper DeInit need to be called.
	nxprd.DeInit()
	fmt.Println("Clean up reader interface - done")
}

func main() {
	c := make(chan os.Signal, 2)
	signal.Notify(c, os.Interrupt, syscall.SIGTERM)
	go func() {
		<-c
		cleanup()
		os.Exit(1)
	}()

	for {
		// Initialize the library. We need to do that once.
		if err := nxprd.Init(); err != nil {
			fmt.Println("\nError: Initializing NXP library failed")
			panic(err)
		}

		// Try to detect/discover a card/tag for 1000ms. Discover will block.
		// 1000ms is the default timeout.
		dev, err := nxprd.Discover(10000)
		if err != nil {
			fmt.Println("\nCouldn't detect card")
		} else {
			if dev.Params.DevType == "Unknown" {
				// A card/tag could be detected, but the wrapper doesn't support it yet.
				// So we can't read or write blocks, but we can access some parameters.
				fmt.Println("\nFound an unknown card")
				fmt.Println("")
				printInfo(dev)
			} else {
				fmt.Println("")
				printInfo(dev)
				time.Sleep(2 * time.Second)
			}
		}
		// In order to cleanup the C part of the wrapper DeInit need to be called.
		nxprd.DeInit()
		fmt.Println("Clean up reader interface - done")
	}
}