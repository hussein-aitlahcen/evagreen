import { Component, Input, OnInit, OnDestroy } from '@angular/core';
import { Router, ActivatedRoute } from '@angular/router';

import { DataService, Data } from '../shared/data.service';
import { AgentService, Agent } from '../shared/agent.service';

@Component({
    moduleId: module.id,
    selector: 'agent-detail',
    templateUrl: 'agent-detail.component.html',
    styleUrls: ['agent-detail.component.css']
})
export class AgentDetailComponent {

    agent: Agent;
    loading: boolean;
    subscribtion: any;
    agentId: Number;
    saving: boolean;
    savingState: Number;
    selectedDataTypeView: Number;
    imageCount: Number;
    measureCount: Number;
    videoCount: Number;

    constructor(private router: Router,
        private route: ActivatedRoute,
        private agentService: AgentService) {
        this.loading = true;
        this.saving = false;
        this.savingState = 0;
    }

    ngOnInit() {
        this.subscribtion = this.route.params.subscribe(params => {
            this.agentId = +params['agentId'];
            this.agentService.find(this.agentId)
                .subscribe(agent => {
                    this.agent = agent;
                    this.loading = false;
                });
        });
    }

    goToImageView(): void {
        this.selectedDataTypeView = Data.DATA_IMAGE;
    }

    goToMeasureView(): void {
        this.selectedDataTypeView = Data.DATA_MEASURE;
    }

    goToVideoView(): void {
        this.selectedDataTypeView = Data.DATA_VIDEO;
    }

    isImageView(): boolean {
        return this.selectedDataTypeView == Data.DATA_IMAGE;
    }

    isMeasureView(): boolean {
        return this.selectedDataTypeView == Data.DATA_MEASURE;
    }

    isVideoView(): boolean {
        return this.selectedDataTypeView == Data.DATA_VIDEO;
    }

    getViewDatas(): Data[] {
        return this.getDatasByType(this.selectedDataTypeView);
    }

    getImageCount(): Number {
        return this.getDatasByType(Data.DATA_IMAGE).length;
    }

    getMeasureCount(): Number {
        return this.getDatasByType(Data.DATA_MEASURE).length;
    }

    getVideoCount(): Number {
        return this.getDatasByType(Data.DATA_VIDEO).length;
    }

    getDatasByType(type: Number): Data[] {
        return this.agent.datas.filter(data => data.type == type);
    }

    saveAgentConfiguration(): void {
        this.saving = true;
        this.savingState = 0;
        this.agentService.update(this.agent)
            .subscribe(state => {
                this.savingState = state;
                this.saving = false;
            })
    }

    ngOnDestroy() {
        this.subscribtion.unsubscribe();
    }
}