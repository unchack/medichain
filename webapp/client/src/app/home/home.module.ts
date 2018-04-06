import { SharedModule } from './../shared/shared.module';
import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { NgbModule } from '@ng-bootstrap/ng-bootstrap';
import { HomeRoutingModule } from './home-routing.module';
import { HomeComponent } from './containers/home/home.component';
import { NgxDatatableModule } from '@swimlane/ngx-datatable';
import {ApiService} from '../shared/services/api.service';
import {KeysPipe} from "../shared/pipes/keys.pipe";

const COMPONENTS = [
  HomeComponent,
];

@NgModule( {
  imports: [
    CommonModule,
    HomeRoutingModule,
    SharedModule,
    NgxDatatableModule,
    NgbModule.forRoot()
  ],
  declarations: [
    ...COMPONENTS,
    KeysPipe
  ],
  providers: [
    ApiService
  ]
} )
export class HomeModule {
}
