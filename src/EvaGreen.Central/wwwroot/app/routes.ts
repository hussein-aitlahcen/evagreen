import { Routes } from '@angular/router';
import { LoginComponent } from './evagreen/login/login.component';
import { DashboardComponent } from './evagreen/dashboard/dashboard.component';
import { AgentDetailComponent } from './evagreen/agent-detail/agent-detail.component';

import { LoggedInGuard } from './evagreen/shared/logged-in.guard';

export const APP_ROUTES: Routes = [
    { path: '', component: LoginComponent },
    { path: 'login', component: LoginComponent },
    { path: 'dashboard', component: DashboardComponent },
    { path: 'agent/detail/:agentId', component: AgentDetailComponent }
]