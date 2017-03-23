import { Component, Input, OnInit } from '@angular/core';

import { UserService } from '../shared/user.service';
import { Data } from '../shared/data.service';
import { Agent } from '../shared/agent.service';

@Component({
    moduleId: module.id,
    selector: 'agent',
    templateUrl: 'agent.component.html',
    styleUrls: ['agent.component.css']
})
export class AgentComponent {

    @Input()
    agent: Agent;

}