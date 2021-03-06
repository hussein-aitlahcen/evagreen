﻿import 'rxjs/Rx';

import { NgModule, ApplicationRef, Compiler } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { HttpModule } from '@angular/http';
import { FormsModule } from '@angular/forms';
import { RouterModule } from '@angular/router';
import { NgbModule } from '@ng-bootstrap/ng-bootstrap';

import { provideAuth } from 'angular2-jwt';

import { APP_ROUTES } from './routes';
import { AppMain } from './evagreen/main.component';
import { LoginComponent } from './evagreen/login/login.component';
import { DashboardComponent } from './evagreen/dashboard/dashboard.component';
import { AgentComponent } from './evagreen/agent/agent.component';
import { AgentDetailComponent } from './evagreen/agent-detail/agent-detail.component';
import { AgentDetailImageComponent } from './evagreen/agent-detail/image/agent-detail-image.component';
import { AgentDetailMeasureComponent } from './evagreen/agent-detail/measure/agent-detail-measure.component';

import { LoggedInGuard } from './evagreen/shared/logged-in.guard';

import { UserService, AUTH_TOKEN_NAME } from './evagreen/shared/user.service';
import { DataService } from './evagreen/shared/data.service';
import { AgentService } from './evagreen/shared/agent.service';

import { OrderByPipe } from './evagreen/shared/order-by.pipe';

import { nvD3 } from 'ng2-nvd3';

@NgModule({
    imports: [
        NgbModule.forRoot(),
        BrowserModule,
        FormsModule,
        HttpModule,
        RouterModule.forRoot(APP_ROUTES)
    ],
    declarations: [
        AppMain,
        LoginComponent,
        DashboardComponent,
        AgentComponent,
        AgentDetailComponent,
        AgentDetailImageComponent,
        AgentDetailMeasureComponent,

        nvD3,

        OrderByPipe
    ],
    providers: [
        provideAuth({
            headerName: 'Authorization',
            headerPrefix: 'Bearer',
            tokenName: AUTH_TOKEN_NAME,
            tokenGetter: () => localStorage.getItem(AUTH_TOKEN_NAME),
            globalHeaders: [{ 'Content-Type': 'application/json' }],
            noJwtError: true,
            noTokenScheme: true
        }),
        LoggedInGuard,
        UserService,
        DataService,
        AgentService
    ],
    entryComponents: [
        AppMain
    ]
})
export class AppModule {
    constructor(private appRef: ApplicationRef, private compiler: Compiler) { }
    ngDoBootstrap() {
        this.compiler.clearCache();
        this.appRef.bootstrap(AppMain);
    }
}