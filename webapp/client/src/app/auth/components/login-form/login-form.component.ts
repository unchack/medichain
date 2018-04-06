import { Component, EventEmitter, HostBinding, Input, OnInit, Output } from '@angular/core';
import { FormBuilder, FormGroup, Validators } from '@angular/forms';
import { Authenticate } from '../../models/user';
import { fadeInAnimation } from '../../../shared/animations/fade-in.animation';

@Component( {
  selector: 'app-login-form',
  animations: [fadeInAnimation],
  templateUrl: './login-form.component.html',
  styleUrls: ['./login-form.component.scss']
} )
export class LoginFormComponent implements OnInit {
  @Input()
  set pending ( isPending: boolean ) {
    this._pending = isPending;
    if ( isPending ) {
      this.form.disable();
    } else {
      this.form.enable();
    }
  }

  _pending: boolean;

  @Input() errorMessage: string | null;

  @Output() submitted = new EventEmitter<Authenticate>();

  // form: FormGroup = this.fb.group( {
  //   username: ['', Validators.required],
  //   password: ['', Validators.required]
  // } );
  form: FormGroup = this.fb.group({
    username: ['enigma@unchain.io', Validators.required],
    password: ['lostintranslation', Validators.required]
  });

  @HostBinding( '@fadeInAnimation' )
  public animatePage = true;

  constructor ( public fb: FormBuilder ) {
  }

  ngOnInit () {
  }

  submit () {
    if ( this.form.valid ) {
      this.submitted.emit( this.form.value );
    }
  }

}
