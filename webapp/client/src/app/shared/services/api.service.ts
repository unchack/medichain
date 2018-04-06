import { Injectable } from '@angular/core';
import {HttpClient, HttpErrorResponse} from '@angular/common/http';
import {Observable} from 'rxjs/Observable';
import {Event} from '../models/event.model';
import 'rxjs/add/observable/empty';
import {environment} from "../../../environments/environment";

@Injectable()
export class ApiService {
  constructor (public http: HttpClient) {}

  query (): Observable<{}|Event[]> {
    return this.http.get(environment.API_ENDPOINT + 'event')
      .map( res => {
        return res as Event[];
      }).catch( ( err: HttpErrorResponse ) => {
        this.handleError( err );
        return Observable.empty<Event[]>();
      });
  }

  handleError(err) {
    // The backend returned an unsuccessful response code.
    console.error(`Backend returned code ${err.status}, body was: ${JSON.stringify(err.error)}`);
    switch (err.status) {
      case 401:
        console.log(err);
        break;
      case 500:
        console.log(err);
        break;
    }
  }
}
