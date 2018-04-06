/*
Copyright IBM Corp. 2017 All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

		 http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
package ca_test

import (
	"crypto/ecdsa"
	"crypto/x509"
	"net"
	"os"
	"path/filepath"
	"testing"

	"github.com/hyperledger/fabric/common/tools/cryptogen/ca"
	"github.com/hyperledger/fabric/common/tools/cryptogen/csp"
	"github.com/stretchr/testify/assert"
)

const (
	testCAName             = "root0"
	testCA2Name            = "root1"
	testName               = "cert0"
	testName2              = "cert1"
	testIP                 = "172.16.10.31"
	testCountry            = "US"
	testProvince           = "California"
	testLocality           = "San Francisco"
	testOrganizationalUnit = "Hyperledger Fabric"
	testStreetAddress      = "testStreetAddress"
	testPostalCode         = "123456"
)

var testDir = filepath.Join(os.TempDir(), "ca-test")

func TestNewCA(t *testing.T) {

	caDir := filepath.Join(testDir, "ca")
	rootCA, err := ca.NewCA(caDir, testCAName, testCAName, testCountry, testProvince, testLocality, testOrganizationalUnit, testStreetAddress, testPostalCode)
	assert.NoError(t, err, "Error generating CA")
	assert.NotNil(t, rootCA, "Failed to return CA")
	assert.NotNil(t, rootCA.Signer,
		"rootCA.Signer should not be empty")
	assert.IsType(t, &x509.Certificate{}, rootCA.SignCert,
		"rootCA.SignCert should be type x509.Certificate")

	// check to make sure the root public key was stored
	pemFile := filepath.Join(caDir, testCAName+"-cert.pem")
	assert.Equal(t, true, checkForFile(pemFile),
		"Expected to find file "+pemFile)

	assert.NotEmpty(t, rootCA.SignCert.Subject.Country, "country cannot be empty.")
	assert.Equal(t, testCountry, rootCA.SignCert.Subject.Country[0], "Failed to match country")
	assert.NotEmpty(t, rootCA.SignCert.Subject.Province, "province cannot be empty.")
	assert.Equal(t, testProvince, rootCA.SignCert.Subject.Province[0], "Failed to match province")
	assert.NotEmpty(t, rootCA.SignCert.Subject.Locality, "locality cannot be empty.")
	assert.Equal(t, testLocality, rootCA.SignCert.Subject.Locality[0], "Failed to match locality")
	assert.NotEmpty(t, rootCA.SignCert.Subject.OrganizationalUnit, "organizationalUnit cannot be empty.")
	assert.Equal(t, testOrganizationalUnit, rootCA.SignCert.Subject.OrganizationalUnit[0], "Failed to match organizationalUnit")
	assert.NotEmpty(t, rootCA.SignCert.Subject.StreetAddress, "streetAddress cannot be empty.")
	assert.Equal(t, testStreetAddress, rootCA.SignCert.Subject.StreetAddress[0], "Failed to match streetAddress")
	assert.NotEmpty(t, rootCA.SignCert.Subject.PostalCode, "postalCode cannot be empty.")
	assert.Equal(t, testPostalCode, rootCA.SignCert.Subject.PostalCode[0], "Failed to match postalCode")

	cleanup(testDir)

}

func TestGenerateSignCertificate(t *testing.T) {

	caDir := filepath.Join(testDir, "ca")
	certDir := filepath.Join(testDir, "certs")
	// generate private key
	priv, _, err := csp.GeneratePrivateKey(certDir)
	assert.NoError(t, err, "Failed to generate signed certificate")

	// get EC public key
	ecPubKey, err := csp.GetECPublicKey(priv)
	assert.NoError(t, err, "Failed to generate signed certificate")
	assert.NotNil(t, ecPubKey, "Failed to generate signed certificate")

	// create our CA
	rootCA, err := ca.NewCA(caDir, testCA2Name, testCA2Name, testCountry, testProvince, testLocality, testOrganizationalUnit, testStreetAddress, testPostalCode)
	assert.NoError(t, err, "Error generating CA")

	cert, err := rootCA.SignCertificate(certDir, testName, nil, ecPubKey,
		x509.KeyUsageDigitalSignature|x509.KeyUsageKeyEncipherment,
		[]x509.ExtKeyUsage{x509.ExtKeyUsageAny})
	assert.NoError(t, err, "Failed to generate signed certificate")
	// KeyUsage should be x509.KeyUsageDigitalSignature | x509.KeyUsageKeyEncipherment
	assert.Equal(t, x509.KeyUsageDigitalSignature|x509.KeyUsageKeyEncipherment,
		cert.KeyUsage)
	assert.Contains(t, cert.ExtKeyUsage, x509.ExtKeyUsageAny)

	cert, err = rootCA.SignCertificate(certDir, testName, nil, ecPubKey,
		x509.KeyUsageDigitalSignature, []x509.ExtKeyUsage{})
	assert.NoError(t, err, "Failed to generate signed certificate")
	assert.Equal(t, 0, len(cert.ExtKeyUsage))

	// make sure sans are correctly set
	sans := []string{testName2, testIP}
	cert, err = rootCA.SignCertificate(certDir, testName, sans, ecPubKey,
		x509.KeyUsageDigitalSignature, []x509.ExtKeyUsage{})
	assert.Contains(t, cert.DNSNames, testName2)
	assert.Contains(t, cert.IPAddresses, net.ParseIP(testIP).To4())

	// check to make sure the signed public key was stored
	pemFile := filepath.Join(certDir, testName+"-cert.pem")
	assert.Equal(t, true, checkForFile(pemFile),
		"Expected to find file "+pemFile)

	_, err = rootCA.SignCertificate(certDir, "empty/CA", nil, ecPubKey,
		x509.KeyUsageKeyEncipherment, []x509.ExtKeyUsage{x509.ExtKeyUsageAny})
	assert.Error(t, err, "Bad name should fail")

	// use an empty CA to test error path
	badCA := &ca.CA{
		Name:     "badCA",
		SignCert: &x509.Certificate{},
	}
	_, err = badCA.SignCertificate(certDir, testName, nil, &ecdsa.PublicKey{},
		x509.KeyUsageKeyEncipherment, []x509.ExtKeyUsage{x509.ExtKeyUsageAny})
	assert.Error(t, err, "Empty CA should not be able to sign")
	cleanup(testDir)

}

func cleanup(dir string) {
	os.RemoveAll(dir)
}

func checkForFile(file string) bool {
	if _, err := os.Stat(file); os.IsNotExist(err) {
		return false
	}
	return true
}