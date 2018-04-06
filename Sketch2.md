## hackathon18:


### prerequisites:

- janus-nfc-writer    1 x instance
- janus-nfc-reader    3 x instance
- janus-hfc           1 x instance
- blockchain-explorer 1 x instance


#### janus-nfc (writer):

- nfc-explorer reads non-changeable nfc-cards-uid

- nfc-explorer creates a secondary hash from destination, source and date and writes that hash to nfc-card

- nfc-explorer creates a primary hash from secondary hash + non-changeable nfc-card-uid and indexes the hash as key in blockchain
with value string containing destination, source and date in clear-text

- it activates authentication on the card

#### janus-nfc (reader):

- authenticates to card tries to read the secondary hash from the card

- tries to rebuild the primary hash from secondary hash on card + non-changeable nfc-card-uid and tries to query that hash on blockchain

- elastic returning 200 or 404

- it then creates a predefined environment aware payload from the validated secondary hash on the nfc-card
and updates key with item hash has passed checkpoint x at time y on blockchain


#### blockchain-explorer:

- sends polling requests to janus-query for new writes to the blockchain

- if janus detects a write to the blockchain the explorer will use the item hash to query for an elastic index key with hash value returned from blockchain

- elastic returns destination, source and date in clear-text on match and angular screen is updated with the event


#### janus-hfc:

- receives polling requests for new writes/history updates


### Storage formats:

#### nfc-card storage format:

card-uid: hash

- hash = destination, source and date


#### blockchain storage format:

hash: destination, source, checkpoints and dates

- hash = hash(destination, source and date) + nfc-card-uid
- checkpoint y = reader-device added environment variable