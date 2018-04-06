export class QueryResponse {
  _event:  Event;

  constructor() {}

  get event () {
    return this._event;
  }

  set event( result: Event ) {
    this._event = result;
  }
}

export class Event {
  _source: string;
  _destination: string;
  _date: string;
  _hash: string;

  constructor() {}

  get source () {
    return this._source;
  }

  set source( source: string ) {
    this._source = source;
  }

  get destination () {
    return this._destination;
  }

  set destination( destination: string ) {
    this._destination = destination;
  }

  get date() {
    return this._date;
  }

  set date( date: string ) {
    this._date = date;
  }

  get hash() {
    return this._hash;
  }

  set hash( hash: string ) {
    this._hash = hash;
  }
}
