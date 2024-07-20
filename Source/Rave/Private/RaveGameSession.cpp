


#include "RaveGameSession.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Online/OnlineSessionNames.h"
#include "OnlineSessionSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveGameSession)

ARaveGameSession::ARaveGameSession()
{
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &ARaveGameSession::OnCreateSessionComplete);
	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &ARaveGameSession::OnFindSessionsComplete);
	OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &ARaveGameSession::OnJoinSessionComplete);
	OnQuickJoinSessionFindCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &ARaveGameSession::OnQuickJoinSessionFindComplete);
	OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &ARaveGameSession::OnDestroySessionComplete);
}

bool ARaveGameSession::HostSession(TWeakObjectPtr<APlayerController> PlayerController, int32 PublicConnections, bool bUseLAN)
{
	FUniqueNetIdPtr UserID;
	if (PlayerController.IsValid() && PlayerController->PlayerState)
	{
		UserID = PlayerController->PlayerState->GetUniqueId().GetUniqueNetId();
	}
	else
	{
		return false;
	}

	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld());
	if (OnlineSubsystem)
	{
		IOnlineSessionPtr OnlineSession = OnlineSubsystem->GetSessionInterface();
		if (OnlineSession.IsValid())
		{
			OnCreateSessionCompleteDelegateHandle = OnlineSession->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);

			FOnlineSessionSettings Settings;
			Settings.NumPublicConnections = PublicConnections;
			Settings.bShouldAdvertise = true;
			Settings.bAllowJoinInProgress = false;
			Settings.bIsLANMatch = bUseLAN;
			Settings.bUsesPresence = true;
			Settings.bAllowJoinViaPresence = true;
			Settings.bUseLobbiesIfAvailable = false;
			Settings.bAllowInvites = true;

			if (OnlineSession->CreateSession(*UserID, NAME_GameSession, Settings))
			{
				return true;
			}
			else
			{
				OnlineSession->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
			}
		}
	}

	return false;
}

bool ARaveGameSession::FindSessions(TWeakObjectPtr<APlayerController> PlayerController, int32 MaxResults, bool bUseLAN)
{
	FUniqueNetIdPtr UserID;
	if (PlayerController.IsValid() && PlayerController->PlayerState)
	{
		UserID = PlayerController->PlayerState->GetUniqueId().GetUniqueNetId();
	}
	else
	{
		return false;
	}

	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld());
	if (OnlineSubsystem)
	{
		IOnlineSessionPtr OnlineSession = OnlineSubsystem->GetSessionInterface();
		if (OnlineSession.IsValid() && UserID.IsValid())
		{
			OnlineSessionSearch = MakeShareable(new FOnlineSessionSearch());
			OnlineSessionSearch->bIsLanQuery = bUseLAN;
			OnlineSessionSearch->MaxSearchResults = MaxResults;

			OnlineSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

			OnFindSessionsCompleteDelegateHandle = OnlineSession->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);
			if (OnlineSession->FindSessions(*UserID, OnlineSessionSearch.ToSharedRef()))
			{
				return true;
			}
			else
			{
				OnlineSession->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
			}
		}
	}

	return false;
}

bool ARaveGameSession::JoinSession(TWeakObjectPtr<APlayerController> PlayerController, const FOnlineSessionSearchResult& SearchResult)
{
	FUniqueNetIdPtr UserID;
	if (PlayerController.IsValid() && PlayerController->PlayerState)
	{
		UserID = PlayerController->PlayerState->GetUniqueId().GetUniqueNetId();
	}
	else
	{
		return false;
	}

	IOnlineSubsystem* OnlineSubsytem = Online::GetSubsystem(GetWorld());
	if (OnlineSubsytem)
	{
		IOnlineSessionPtr OnlineSession = OnlineSubsytem->GetSessionInterface();
		if (OnlineSession.IsValid() && UserID.IsValid())
		{
			OnJoinSessionCompleteDelegateHandle = OnlineSession->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);
			if (OnlineSession->JoinSession(*UserID, NAME_GameSession, SearchResult))
			{
				return true;
			}
			else
			{
				OnlineSession->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
			}
		}
	}

	return false;
}

bool ARaveGameSession::QuickJoinSession(TWeakObjectPtr<APlayerController> PlayerController, bool bUseLAN)
{
	FUniqueNetIdPtr UserID;
	if (PlayerController.IsValid() && PlayerController->PlayerState)
	{
		UserID = PlayerController->PlayerState->GetUniqueId().GetUniqueNetId();
	}
	else
	{
		return false;
	}

	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld());
	if (OnlineSubsystem)
	{
		IOnlineSessionPtr OnlineSession = OnlineSubsystem->GetSessionInterface();
		if (OnlineSession.IsValid() && UserID.IsValid())
		{
			OnlineSessionSearch = MakeShareable(new FOnlineSessionSearch());
			OnlineSessionSearch->bIsLanQuery = bUseLAN;
			OnlineSessionSearch->MaxSearchResults = 50000;

			OnlineSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
			
			OnQuickJoinSessionFindCompleteDelegateHandle = OnlineSession->AddOnFindSessionsCompleteDelegate_Handle(OnQuickJoinSessionFindCompleteDelegate);
			if (OnlineSession->FindSessions(*UserID, OnlineSessionSearch.ToSharedRef()))
			{
				return true;
			}
			else
			{
				OnlineSession->ClearOnFindSessionsCompleteDelegate_Handle(OnQuickJoinSessionFindCompleteDelegateHandle);
			}
		}
	}

	return false;
}

bool ARaveGameSession::StartSession()
{
	IOnlineSubsystem* OnlineSubsytem = Online::GetSubsystem(GetWorld());
	if (OnlineSubsytem)
	{
		IOnlineSessionPtr OnlineSessionPtr = OnlineSubsytem->GetSessionInterface();
		if (OnlineSessionPtr.IsValid())
		{
			if (OnlineSessionPtr->StartSession(NAME_GameSession))
			{
				return true;
			}
		}
	}

	return false;
}

bool ARaveGameSession::EndSession()
{
	IOnlineSubsystem* OnlineSubsytem = Online::GetSubsystem(GetWorld());
	if (OnlineSubsytem)
	{
		IOnlineSessionPtr OnlineSessionPtr = OnlineSubsytem->GetSessionInterface();
		if (OnlineSessionPtr.IsValid())
		{
			if (OnlineSessionPtr->EndSession(NAME_GameSession))
			{
				return true;
			}
		}
	}

	return false;
}

bool ARaveGameSession::DestroySession(TWeakObjectPtr<APlayerController> PlayerController)
{
	if (!PlayerController.IsValid() || !PlayerController->PlayerState)
	{
		return false;
	}

	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld());
	if (OnlineSubsystem)
	{
		IOnlineSessionPtr OnlineSession = OnlineSubsystem->GetSessionInterface();
		if (OnlineSession.IsValid())
		{
			OnDestroySessionCompleteDelegateHandle = OnlineSession->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);
			if (OnlineSession->DestroySession(NAME_GameSession))
			{
				return true;
			}
			else
			{
				OnlineSession->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
			}
		}
	}

	return false;
}

EOnlineAsyncTaskState::Type ARaveGameSession::GetSearchResultStatus(int32& NumSearchResults)
{
	NumSearchResults = 0;

	if (OnlineSessionSearch.IsValid())
	{
		if (OnlineSessionSearch->SearchState == EOnlineAsyncTaskState::Done)
		{
			NumSearchResults = OnlineSessionSearch->SearchResults.Num();
		}
		return OnlineSessionSearch->SearchState;
	}

	return EOnlineAsyncTaskState::NotStarted;
}

TArray<FOnlineSessionSearchResult>& ARaveGameSession::GetSessionSearchResults() const
{
	return OnlineSessionSearch->SearchResults;
}

void ARaveGameSession::OnCreateSessionComplete(FName InSessionName, bool bWasSuccessful)
{
	UE_LOG(LogOnlineGame, Verbose, TEXT("OnCreateSessionComplete %s bSuccess: %d"), *InSessionName.ToString(), bWasSuccessful);

	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld());
	if (OnlineSubsystem)
	{
		IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
		Sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
	}
}

void ARaveGameSession::OnFindSessionsComplete(bool bWasSuccessful)
{
	UE_LOG(LogOnlineGame, Verbose, TEXT("OnFindSessionsComplete bSuccess: %d"), bWasSuccessful);

	IOnlineSubsystem* const OnlineSubsystem = Online::GetSubsystem(GetWorld());
	if (OnlineSubsystem)
	{
		IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
		if (Sessions.IsValid())
		{
			UE_LOG(LogOnlineGame, Verbose, TEXT("Num Search Results: %d"), OnlineSessionSearch->SearchResults.Num());

			Sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);

			for (int32 SearchIdx = 0; SearchIdx < OnlineSessionSearch->SearchResults.Num(); SearchIdx++)
			{
				const FOnlineSessionSearchResult& SearchResult = OnlineSessionSearch->SearchResults[SearchIdx];
				DumpSession(&SearchResult.Session);
			}
		}
	}
}

void ARaveGameSession::OnJoinSessionComplete(FName InSessionName, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogOnlineGame, Verbose, TEXT("OnJoinSessionComplete %s bSuccess: %d"), *InSessionName.ToString(), static_cast<int32>(Result));

	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld());
	if (OnlineSubsystem)
	{
		IOnlineSessionPtr OnlineSessionPtr = OnlineSubsystem->GetSessionInterface();
		if (OnlineSessionPtr.IsValid())
		{
			OnlineSessionPtr->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
			if (Result == EOnJoinSessionCompleteResult::Success)
			{
				// Client travel to the server
				FString ConnectInfo;
				if (OnlineSessionPtr->GetResolvedConnectString(NAME_GameSession, ConnectInfo))
				{
					GetGameInstance()->GetFirstLocalPlayerController()->ClientTravel(ConnectInfo, TRAVEL_Absolute);
				}
			}
		}
	}
}

void ARaveGameSession::OnQuickJoinSessionFindComplete(bool bWasSuccessful)
{
	UE_LOG(LogOnlineGame, Verbose, TEXT("OnFindSessionsComplete bSuccess: %d"), bWasSuccessful);

	IOnlineSubsystem* const OnlineSubsystem = Online::GetSubsystem(GetWorld());
	if (OnlineSubsystem)
	{
		IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
		if (Sessions.IsValid())
		{
			UE_LOG(LogOnlineGame, Verbose, TEXT("Num Search Results: %d"), OnlineSessionSearch->SearchResults.Num());

			Sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnQuickJoinSessionFindCompleteDelegateHandle);

			for (int32 SearchIdx = 0; SearchIdx < OnlineSessionSearch->SearchResults.Num(); SearchIdx++)
			{
				const FOnlineSessionSearchResult& SearchResult = OnlineSessionSearch->SearchResults[SearchIdx];
				DumpSession(&SearchResult.Session);
			}

			if (OnlineSessionSearch->SearchResults.Num() > 0)
			{
				JoinSession(GetGameInstance()->GetFirstLocalPlayerController(), OnlineSessionSearch->SearchResults[0]);
			}
		}
	}
}

void ARaveGameSession::OnDestroySessionComplete(FName InSessionName, bool bWasSuccessful)
{
	UE_LOG(LogOnlineGame, Verbose, TEXT("OnDestroySessionComplete %s bSuccess: %d"), *InSessionName.ToString(), bWasSuccessful);

	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld());
	if (OnlineSubsystem)
	{
		if (IOnlineSessionPtr OnlineSessionPtr = OnlineSubsystem->GetSessionInterface())
		{
			OnlineSessionPtr->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
		}
	}
}
