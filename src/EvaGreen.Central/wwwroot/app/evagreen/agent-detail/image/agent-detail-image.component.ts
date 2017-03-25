import { Component, Input } from '@angular/core';
import { DataService, Data } from '../../shared/data.service';
import { AgentService, Agent } from '../../shared/agent.service';

@Component({
    moduleId: module.id,
    selector: 'agent-detail-image',
    templateUrl: 'agent-detail-image.component.html',
    styleUrls: ['agent-detail-image.component.css']
})
export class AgentDetailImageComponent {
    @Input()
    images: Data[];
}