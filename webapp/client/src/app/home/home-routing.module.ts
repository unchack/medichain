import { HomeComponent } from './containers/home/home.component';
import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';

const routes: Routes = [
  // {path: '', redirectTo: 'home', pathMatch: 'full'},
  { path: '', component: HomeComponent },
];

@NgModule( {
  imports: [RouterModule.forChild( routes )],
  exports: [RouterModule]
} )
export class HomeRoutingModule {
}
