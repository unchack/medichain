import { RouterModule } from '@angular/router';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { ToastyModule } from 'ng2-toasty';
import { ModalModule } from 'ngx-bootstrap/modal';
import { BsDropdownModule } from 'ngx-bootstrap/dropdown';
import { TruncatePipe } from './pipes/truncate.pipe';

@NgModule( {
  imports: [
    CommonModule,
    ToastyModule.forRoot(),
    ModalModule.forRoot(),
    BsDropdownModule.forRoot(),
  ],
  declarations: [
    TruncatePipe,
  ],
  providers: [],
  exports: [
    FormsModule,
    ReactiveFormsModule,
    RouterModule,
    ToastyModule,
    ModalModule,
    BsDropdownModule,
    TruncatePipe,
  ]
} )
export class SharedModule {
}
