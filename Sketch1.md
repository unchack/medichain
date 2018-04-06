## hackathon18:

### prerequisites:

- janus-nfc-writer    1 x instance
- janus-nfc-reader    3 x instance
- janus-hfc           1 x instance
- blockchain-explorer 1 x instance


#### janus-nfc:

#### janus-nfc-writer:
- creates a hash from desired destination, source and date and writes that hash to nfc-card
- sends that hash as key on the blockchain with value json containing destination, source and date in clear-text
- it activates authentication on the card

#### janus-nfc-reader:
- authenticates to card tries to read the hash from the card

- tries to query that hash on the blockchain

- blockchain returning 200 or 404

- it then updates that key with property checkpoint x passed at time y if match was found on the bloackchain

#### janus-hfc:

- receives polling requests for new writes/history updates

#### blockchain-explorer:

- explorer sends polling requests to janus-hfc for new writes/history updates

- blockchain returns destination, source, passed checkpoints and dates in clear-text and angular screen is updated with the events and full history as they occur

### Storage formats:

#### nfc-card storage format:

card-uid: hash

- hash = destination, source and date


#### blockchain storage format:

hash: destination, source, checkpoints and dates

- hash = hash(destination, source and date) + nfc-card-uid
- checkpoint y = reader-device added environment variable