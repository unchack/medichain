import { AfterViewInit, Component, HostBinding, OnDestroy, OnInit, ViewChild } from '@angular/core';
import { FormBuilder, FormGroup, Validators } from '@angular/forms';
import { NgbCalendar, NgbDateStruct, NgbTimeStruct } from '@ng-bootstrap/ng-bootstrap';
import {
  QueryResponse,
  Event
} from '../../../shared/models/event.model';
import { Subscription } from 'rxjs/Subscription';
import { ApiService } from '../../../shared/services/api.service';
import { DatatableComponent } from '@swimlane/ngx-datatable';
import { fadeInAnimation } from '../../../shared/animations/fade-in.animation';

@Component( {
  selector: 'app-home',
  animations: [fadeInAnimation],
  templateUrl: './home.component.html',
  styleUrls: ['./home.component.scss']
} )
export class HomeComponent implements OnInit, OnDestroy, AfterViewInit {
  @HostBinding( '@fadeInAnimation' )
  public animatePage              = true;
  @ViewChild( 'table' ) private table: DatatableComponent;
  readonly headerHeight                                  = 50;
  readonly rowHeight                                     = 50;
  readonly pageLimit                                     = 15;
  querySubscription: Subscription;
  subscribed               = false;
  rows                     = [];
  rowIndex: number;
  loadingIndicator         = false;
  reorderable              = true;

  constructor ( private formBuilder: FormBuilder, private apiService: ApiService ) {
    this.rows[0] = {
      destination: "Singapor",
      source: "Amsterdam",
      date: "today"
    }
  }

  ngOnInit () {}

  ngAfterViewInit () {}


  query(): void {
    this.loadingIndicator   = true;

    this.querySubscription = this.apiService.query().subscribe( ( data: QueryResponse ) => {
      if (data.event) {
        console.log(data.event)
      }
    });
  }

  toggleExpandRow ( row ) {
    this.table.rowDetail.toggleExpandRow( row );
  }

  ngOnDestroy (): void {
    if ( this.subscribed ) {
      this.querySubscription.unsubscribe();
    }
  }
}
